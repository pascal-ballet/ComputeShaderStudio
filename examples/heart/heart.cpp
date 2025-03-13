#define POINT_COUNT 8

vec2 points[POINT_COUNT];
const float speed = -0.5;
const float len = 0.25;
const float scale = 0.012;
float intensity = 1.3;
float radius = 0.015;

// Fonction mathématique pour tracer un cœur inversé
vec2 getHeartPosition(float t) {
    return vec2(16.0 * sin(t) * sin(t) * sin(t),
                (13.0 * cos(t) - 5.0 * cos(2.0*t)
                - 2.0 * cos(3.0*t) - cos(4.0*t))); // Inversion du signe pour inverser le cœur
}

float getGlow(float dist, float radius, float intensity) {
    return pow(radius / dist, intensity);
}

float getSegment(float t, vec2 pos, float offset) {
    for (int i = 0; i < POINT_COUNT; i++) {
        points[i] = getHeartPosition(offset + float(i) * len + fract(speed * t) * 6.28);
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
    float dist = getSegment(t, pos, 0.0);
    float glow = getGlow(dist, radius, intensity);
    
    vec3 col = vec3(0.0);
<<<<<<< Updated upstream
    col += 10.0 * vec3(smoothstep(0.006, 0.003, dist)); 
    col += glow * vec3(1.0, 0.5, 0.0);  // bleu clair
=======
    
    // Couleurs : rose et bleu
    vec3 pinkColor = vec3(0.6, 0.4, 2.0);
    vec3 blueColor = vec3(1.0, 0.5, 0.0);
>>>>>>> Stashed changes




    dist = getSegment(t, pos, 3.4);
    glow = getGlow(dist, radius, intensity);
    col += 10.0 * vec3(smoothstep(0.006, 0.003, dist));
    col += glow * vec3(0.6, 0.4, 2.0); // rose 

    col = 1.0 - exp(-col);
    col = pow(col, vec3(0.4545));

    int r = int(255.0 * col.r);
    int g = int(255.0 * col.g);
    int b = int(255.0 * col.b);
    int color = (0xFF << 24) | (r << 16) | (g << 8) | b;

    data_0[p] = color;
}


// comment 