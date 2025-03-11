 // Compute Shader pour un effet de cœur lumineux en Godot 4

@compute
shader_type compute;

// Texture de sortie
layout(rgba32f) uniform image2D Result;

// Paramètres uniformes
uniform float iTime;
uniform vec2 iResolution;

#define POINT_COUNT 8
#define PI 3.14159265359

const float speed = -0.5;
const float len = 0.25;
const float scale = 0.012;
const float intensity = 1.3;
const float radius = 0.015;

vec2 getHeartPosition(float t) {
    return vec2(
        16.0 * pow(sin(t), 3.0),
        -(13.0 * cos(t) - 5.0 * cos(2.0 * t) - 2.0 * cos(3.0 * t) - cos(4.0 * t))
    );
}

float getGlow(float dist, float radius, float intensity) {
    return pow(radius / max(dist, 0.0001), intensity);
}

float getSegment(float t, vec2 pos, float offset) {
    vec2 points[POINT_COUNT];
    for (int i = 0; i < POINT_COUNT; i++) {
        points[i] = getHeartPosition(offset + float(i) * len + mod(speed * t, 6.28));
    }
    
    vec2 c = (points[0] + points[1]) / 2.0;
    vec2 c_prev;
    float dist = 10000.0;
    
    for (int i = 0; i < POINT_COUNT - 1; i++) {
        c_prev = c;
        c = (points[i] + points[i + 1]) / 2.0;
        vec2 d = pos - scale * c;
        dist = min(dist, length(d));
    }
    return max(0.0, dist);
}

// Fonction principale du Compute Shader
void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    if (x >= uint(iResolution.x) || y >= uint(iResolution.y)) return;
    
    vec2 uv = vec2(x, y) / iResolution;
    float widthHeightRatio = iResolution.x / iResolution.y;
    vec2 centre = vec2(0.5, 0.5);
    vec2 pos = centre - uv;
    pos.y /= widthHeightRatio;
    pos.y += 0.03;
    
    float t = iTime;
    float dist = getSegment(t, pos, 0.0);
    float glow = getGlow(dist, radius, intensity);
    
    vec3 col = vec3(0.0);
    col += 10.0 * smoothstep(0.006, 0.003, dist);
    col += glow * vec3(1.0, 0.05, 0.3);
    
    dist = getSegment(t, pos, 3.4);
    glow = getGlow(dist, radius, intensity);
    col += 10.0 * smoothstep(0.006, 0.003, dist);
    col += glow * vec3(0.1, 0.4, 1.0);
    
    col = 1.0 - exp(-col);
    col = pow(col, vec3(0.4545));
    
    imageStore(Result, ivec2(x, y), vec4(col, 1.0));
}
