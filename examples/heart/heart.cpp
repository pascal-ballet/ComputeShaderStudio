#define POINT_COUNT 12  // Plus de points pour un tracé lisse
#define HEART_COUNT 4   // Nombre total de cœurs

vec2 points[POINT_COUNT];
const float speed = -0.2;
const float len = 0.2;
const float scale = 0.012;

// Ajustements pour éviter la saturation
float intensity = 1.0;     // Intensité du glow
float radius = 0.015;      // Rayon du halo
float lineWidth = 0.004;   // Largeur du trait

// Fonction mathématique pour tracer un cœur
vec2 getHeartPosition(float t) {
    return vec2(
        16.0 * sin(t) * sin(t) * sin(t),
        13.0 * cos(t) - 5.0 * cos(2.0 * t)
        - 2.0 * cos(3.0 * t) - cos(4.0 * t)
    );
}

// Fonction de glow
float getGlow(float dist, float radius, float intensity) {
    return pow(radius / max(dist, 0.001), intensity);
}

// Calcul de la distance à un segment
float distToLine(vec2 p, vec2 a, vec2 b) {
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}

// Calcul de la distance au “trait” du cœur
float getSegment(float t, vec2 pos, float offset) {
    for (int i = 0; i < POINT_COUNT; i++) {
        points[i] = getHeartPosition(
            offset + float(i) * len + fract(speed * t) * 6.28
        );
    }
    
    float dist = 10000.0;
    for (int i = 0; i < POINT_COUNT - 1; i++) {
        float segDist = distToLine(pos, scale * points[i], scale * points[i + 1]);
        dist = min(dist, segDist);
    }
    return dist;
}

void main() {
    // Position du pixel en X/Y
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
    // Coordonnées [0..1]
    vec2 uv = vec2(float(x) / float(WSX), float(y) / float(WSY));
    float widthHeightRatio = float(WSX) / float(WSY);
    
    // On centre la scène
    vec2 centre = vec2(0.5, 0.5);
    vec2 pos = centre - uv;
    pos.y /= widthHeightRatio;
    pos.y += 0.03;
    
    float t = float(step) / 60.0;
    
    // Fond noir
    vec3 col = vec3(0.0);
    
    // Couleurs : rose et bleu
    vec3 pinkColor = vec3(1.0, 0.0, 0.6);
    vec3 blueColor = vec3(0.0, 0.5, 1.0);

    // On dessine HEART_COUNT cœurs
    for (int i = 0; i < HEART_COUNT; i++) {
        float offset = float(i) * 3.4;
        
        // Distance au tracé du cœur
        float dist = getSegment(t, pos, offset);
        
        // Trait lumineux
        float line = smoothstep(lineWidth + 0.001, lineWidth - 0.001, dist);
        
        // Halo
        float glow = getGlow(dist, radius, intensity);
        
        // Alterne rose / bleu
        vec3 heartColor = (i % 2 == 0) ? pinkColor : blueColor;
        
        // Ajout du trait
        col += line * heartColor;
        
        // Ajout du halo
        col += glow * heartColor;
    }
    
    // On limite la valeur maximale (pour éviter la sur-saturation)
    col = clamp(col, 0.0, 3.0);

    // Tone mapping et correction gamma
    col = 1.0 - exp(-col);
    col = pow(col, vec3(0.4545));
    
    // Conversion vers int
    int r = int(255.0 * col.r);
    int g = int(255.0 * col.g);
    int b = int(255.0 * col.b);
    int color = (0xFF << 24) | (r << 16) | (g << 8) | b;
    
    // On écrit la couleur finale
    data_0[p] = color;
}
