 // Compute shader 2D pour Godot 4.4, inspiré de Gelami

#define POINT_COUNT 8

vec2 points[POINT_COUNT];
const float speed = -0.5;
const float len   = 0.25;
const float baseScale = 0.012;
float intensity = 1.3;
float radius    = 0.015;

// Distance signée à une courbe de Bézier quadratique
float sdBezier(vec2 pos, vec2 A, vec2 B, vec2 C) {    
    vec2 a = B - A;
    vec2 b = A - 2.0 * B + C;
    vec2 c = a * 2.0;
    vec2 d = A - pos;

    float kk = 1.0 / dot(b, b);
    float kx = kk * dot(a, b);
    float ky = kk * (2.0 * dot(a, a) + dot(d, b)) / 3.0;
    float kz = kk * dot(d, a);      

    float res = 0.0;
    float p = ky - kx * kx;
    float p3 = p * p * p;
    float q = kx * (2.0 * kx * kx - 3.0 * ky) + kz;
    float h = q * q + 4.0 * p3;

    if (h >= 0.0) { 
        h = sqrt(h);
        vec2 x = (vec2(h, -h) - q) / 2.0;
        vec2 uv = sign(x) * pow(abs(x), vec2(1.0/3.0));
        float t = uv.x + uv.y - kx;
        t = clamp(t, 0.0, 1.0);
        vec2 qos = d + (c + b * t) * t;
        res = length(qos);
    } else {
        float z = sqrt(-p);
        float v = acos(q / (p * z * 2.0)) / 3.0;
        float m = cos(v);
        float n = sin(v) * 1.732050808;
        vec3 t = vec3(m + m, -n - m, n - m) * z - kx;
        t = clamp(t, 0.0, 1.0);
        vec2 qos = d + (c + b * t.x) * t.x;
        float dis = dot(qos, qos);
        res = dis;
        qos = d + (c + b * t.y) * t.y;
        dis = dot(qos, qos);
        res = min(res, dis);
        qos = d + (c + b * t.z) * t.z;
        dis = dot(qos, qos);
        res = min(res, dis);
        res = sqrt(res);
    }
    
    return res;
}

// Position sur la courbe du cœur (paramétrisation classique)
vec2 getHeartPosition(float t) {
    return vec2(16.0 * sin(t) * sin(t) * sin(t),
                -(13.0 * cos(t) - 5.0 * cos(2.0 * t)
                  - 2.0 * cos(3.0 * t) - cos(4.0 * t)));
}

// Calcul du glow en fonction de la distance à la courbe
float getGlow(float dist, float radius, float intensity) {
    return pow(radius / max(dist, 0.0001), intensity);
}

// Calcule la distance minimale d'un segment de la courbe, en utilisant des courbes de Bézier pour adoucir le contour.
// Le paramètre scaleFactor permet de faire varier la taille du segment.
float getSegment(float t, vec2 pos, float offset, float scaleFactor) {
    for (int i = 0; i < POINT_COUNT; i++) {
        points[i] = getHeartPosition(offset + float(i) * len + fract(speed * t) * 6.28);
    }
    
    vec2 c = (points[0] + points[1]) / 2.0;
    vec2 c_prev;
    float dist = 10000.0;
    
    for (int i = 0; i < POINT_COUNT - 1; i++) {
        c_prev = c;
        c = (points[i] + points[i+1]) / 2.0;
        dist = min(dist, sdBezier(pos, scaleFactor * c_prev, scaleFactor * points[i], scaleFactor * c));
    }
    return max(0.0, dist);
}

void main() {
    uint ix = gl_GlobalInvocationID.x;
    uint iy = gl_GlobalInvocationID.y;
    int p = int(ix + iy * WSX);
    
    // Coordonnées normalisées et centrage
    vec2 fragCoord = vec2(float(ix), float(iy));
    vec2 uv = fragCoord / vec2(float(WSX), float(WSY));
    float aspect = float(WSX) / float(WSY);
    vec2 centre = vec2(0.5, 0.5);
    vec2 pos = centre - uv;
    pos.y /= aspect;
    pos.y += 0.03;
    
    // Temps (utilisé pour l'animation)
    float t = float(step);
    
    // Effet de pulsation : variation de la taille du cœur
    float pulse = 1.0 + 0.15 * sin(t * 2.0);
    float dynamicScale = baseScale * pulse;
    // On fait aussi varier le rayon du glow pour renforcer l'animation
    float dynamicRadius = radius * (1.0 + 0.2 * sin(t * 3.0));
    
    // Offsets animés pour les deux segments
    float offset0 = 0.0 + 0.1 * sin(t * 1.5);
    float offset1 = 3.4 + 0.1 * cos(t * 1.3);
    
    vec3 col = vec3(0.0);
    
    // Premier segment : glow rose
    float dist1 = getSegment(t, pos, offset0, dynamicScale);
    float glow1 = getGlow(dist1, dynamicRadius, intensity);
    // Ajout d'un noyau blanc (pour le cœur) et du glow rose
    col += 10.0 * vec3(smoothstep(0.006, 0.003, dist1));
    col += glow1 * vec3(1.0, 0.05, 0.3);
    
    // Deuxième segment : glow bleu
    float dist2 = getSegment(t, pos, offset1, dynamicScale);
    float glow2 = getGlow(dist2, dynamicRadius, intensity);
    col += 10.0 * vec3(smoothstep(0.006, 0.003, dist2));
    col += glow2 * vec3(0.1, 0.4, 1.0);
        
    // Tone mapping et correction gamma pour un rendu lumineux
    col = 1.0 - exp(-col);
    col = pow(col, vec3(0.4545));
    
    int a = 255;
    int rInt = int(clamp(col.r, 0.0, 1.0) * 255.0);
    int gInt = int(clamp(col.g, 0.0, 1.0) * 255.0);
    int bInt = int(clamp(col.b, 0.0, 1.0) * 255.0);
    data_0[p] = (a << 24) | (rInt << 16) | (gInt << 8) | bInt;
}
