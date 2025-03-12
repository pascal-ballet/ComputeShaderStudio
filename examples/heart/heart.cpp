@compute

// Layout pour Godot Compute Shader
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Textures pour l'entrée/sortie des données
layout(rgba32f, binding = 0) uniform writeonly image2D outputTexture;

// Uniforms pour la résolution et le temps
uniform vec2 resolution;
uniform float time;

#define POINT_COUNT 8
const float speed = -0.5;
const float len = 0.25;
const float scale = 0.012;
const float intensity = 1.3;
const float radius = 0.015;

// Distance signée à une courbe quadratique de Bézier
float sdBezier(vec2 pos, vec2 A, vec2 B, vec2 C) {
    vec2 a = B - A;
    vec2 b = A - 2.0 * B + C;
    vec2 c = a * 2.0;
    vec2 d = A - pos;
    float kk = 1.0 / dot(b, b);
    float kx = kk * dot(a, b);
    float ky = kk * (2.0 * dot(a, a) + dot(d, b)) / 3.0;
    float kz = kk * dot(d, a);
    float p = ky - kx * kx;
    float q = kx * (2.0 * kx * kx - 3.0 * ky) + kz;
    float h = q * q + 4.0 * p * p * p;
    float res;

    if (h >= 0.0) {
        h = sqrt(h);
        vec2 x = (vec2(h, -h) - q) / 2.0;
        vec2 uv = sign(x) * pow(abs(x), vec2(1.0 / 3.0));
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

// Fonction pour obtenir la position d'un cœur
vec2 getHeartPosition(float t) {
    return vec2(16.0 * sin(t) * sin(t) * sin(t),
                -(13.0 * cos(t) - 5.0 * cos(2.0 * t)
                - 2.0 * cos(3.0 * t) - cos(4.0 * t)));
}

// Fonction pour créer un effet de glow
float getGlow(float dist, float radius, float intensity) {
    return pow(radius / dist, intensity);
}

// Fonction pour calculer un segment du cœur
float getSegment(float t, vec2 pos, float offset) {
    vec2 points[POINT_COUNT];
    for (int i = 0; i < POINT_COUNT; i++) {
        points[i] = getHeartPosition(offset + float(i) * len + fract(speed * t) * 6.28);
    }
    
    vec2 c = (points[0] + points[1]) / 2.0;
    vec2 c_prev;
    float dist = 10000.0;

    for (int i = 0; i < POINT_COUNT - 1; i++) {
        c_prev = c;
        c = (points[i] + points[i + 1]) / 2.0;
        dist = min(dist, sdBezier(pos, scale * c_prev, scale * points[i], scale * c));
    }

    return max(0.0, dist);
}

void main() {
    // Récupère la position du pixel actuel
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    if (pixelCoords.x >= int(resolution.x) || pixelCoords.y >= int(resolution.y)) {
        return;
    }

    vec2 fragCoord = vec2(pixelCoords);
    vec2 uv = fragCoord / resolution;
    float widthHeightRatio = resolution.x / resolution.y;
    vec2 centre = vec2(0.5, 0.5);
    vec2 pos = centre - uv;
    pos.y /= widthHeightRatio;

    // Ajustement pour bien centrer le cœur
    pos.y += 0.03;

    float t = time;

    // Calcul du premier segment du cœur
    float dist = getSegment(t, pos, 0.0);
    float glow = getGlow(dist, radius, intensity);

    vec3 col = vec3(0.0);

    // Couleur blanche au centre
    col += 10.0 * vec3(smoothstep(0.006, 0.003, dist));
    // Glow rose
    col += glow * vec3(1.0, 0.05, 0.3);

    // Calcul du second segment du cœur
    dist = getSegment(t, pos, 3.4);
    glow = getGlow(dist, radius, intensity);

    // Couleur blanche
    col += 10.0 * vec3(smoothstep(0.006, 0.003, dist));
    // Glow bleu
    col += glow * vec3(0.1, 0.4, 1.0);

    // Tone Mapping
    col = 1.0 - exp(-col);

    // Correction gamma
    col = pow(col, vec3(0.4545));

    // Stockage du pixel dans la texture de sortie
    imageStore(outputTexture, pixelCoords, vec4(col, 1.0));
}
