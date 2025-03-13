#define POINT_COUNT 8
#define HEART_COUNT 6  // Nombre de cœurs affichés

vec2 points[POINT_COUNT];
const float speed = -0.4;
const float len   = 0.25;
const float scale = 0.015;
float intensity   = 1.8;
float radius      = 0.02;

// Fonction pour dessiner un cœur stylisé
vec2 getHeartPosition(float t) {
    return vec2(
        16.0 * sin(t) * sin(t) * sin(t),
        13.0 * cos(t) - 5.0 * cos(2.0 * t)
        - 2.0 * cos(3.0 * t) - cos(4.0 * t)
    );
}

// Glow plus fluide et magique
float getGlow(float dist, float radius, float intensity) {
    return pow(smoothstep(radius * 2.2, radius, dist), intensity);
}

// Calcule la distance au segment du cœur
float getSegment(float t, vec2 pos, float offset) {
    for (int i = 0; i < POINT_COUNT; i++) {
        points[i] = getHeartPosition(
            offset + float(i) * len + fract(speed * t) * 6.28
        );
    }

    vec2 c = (points[0] + points[1]) / 2.0;
    vec2 c_prev;
    float dist = 10000.0;

    for (int i = 0; i < POINT_COUNT - 1; i++) {
        c_prev = c;
        c = (points[i] + points[i + 1]) / 2.0;
        dist = min(dist, length(pos - scale * c));
    }
    return max(0.0, dist);
}

// Fonction de bruit pour un effet scintillant
float getNoise(vec2 uv) {
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    vec2 uv = vec2(float(x) / float(WSX), float(y) / float(WSY));
    float widthHeightRatio = float(WSX) / float(WSY);
    vec2 centre = vec2(0.5, 0.5);
    vec2 pos = centre - uv;
    pos.y /= widthHeightRatio;
    pos.y += 0.03;

    float t = float(step) / 60.0;
    
    // Effet de pulsation pour un rendu vivant
    float pulse = 0.5 + 0.5 * sin(3.0 * t);
    float dynamicRadius = mix(0.015, 0.025, pulse);

    // Couleur accumulée
    vec3 col = vec3(0.0);

    // **Ajout de plusieurs cœurs avec des positions différentes**
    for (int i = 0; i < HEART_COUNT; i++) {
        float offset = float(i) * 2.3;  // Décalage des cœurs
        vec2 heartPos = pos + vec2(sin(t + float(i) * 1.5) * 0.3, cos(t + float(i) * 1.8) * 0.2);
        float dist = getSegment(t, heartPos, offset);
        float glow = getGlow(dist, dynamicRadius, intensity);

        // **Dégradé magique entre bleu néon et rose pastel**
        vec3 heartColor = mix(vec3(0.4, 0.8, 1.0), vec3(1.0, 0.5, 0.8), smoothstep(0.0, 0.6, dist));
        
        // Ajout du glow et du cœur lumineux
        col += 15.0 * vec3(smoothstep(0.008, 0.004, dist));
        col += glow * heartColor;
    }

    // **Ajout d’un fond ultra doux et onirique**
    vec3 backgroundColor = mix(vec3(0.1, 0.0, 0.3), vec3(0.3, 0.0, 0.5), uv.y * 1.2);
    col = mix(backgroundColor, col, smoothstep(0.2, 0.6, length(pos)));

    // **Léger scintillement pour un effet féerique**
    float noise = getNoise(uv + t * 0.1);
    col += 0.03 * noise;

    // Tone mapping et correction gamma
    col = 1.0 - exp(-col);
    col = pow(col, vec3(0.45));

    int r = int(255.0 * col.r);
    int g = int(255.0 * col.g);
    int b = int(255.0 * col.b);
    int color = (0xFF << 24) | (r << 16) | (g << 8) | b;

    data_0[p] = color;
}
