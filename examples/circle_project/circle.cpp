float PI = 3.14159;

float sdSphere(vec3 p, float s) {
    return length(p) - s;
}

int packARGB(vec3 c) {
    int R = int(clamp(c.x, 0.0, 1.0) * 255.0);
    int G = int(clamp(c.y, 0.0, 1.0) * 255.0);
    int B = int(clamp(c.z, 0.0, 1.0) * 255.0);
    int A = 255;
    return (A << 24) | (R << 16) | (G << 8) | B;
}

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    if(x >= WSX || y >= WSY) return;
    uint index = x + y * WSX;
    
    vec2 fragCoord = vec2(float(x), float(y));
    vec2 iResolution = vec2(float(WSX), float(WSY));
    float t = float(step) * 0.01;
    
    vec2 uv = (fragCoord - 0.5 * iResolution) / iResolution.y;
    vec3 col = vec3(0.0);
    vec3 camPos = vec3(0.0, 0.0, 3.5);
    vec3 rayDir = normalize(vec3(uv, -1.0));
    float depth = 0.0;
    vec3 p = vec3(0.0);
    float d = 0.0;
    
    for (int i = 0; i < 50; i++){
        p = camPos + depth * rayDir;
        d = sdSphere(p, 1.25);
        depth += d;
        if (d < 0.001 || depth > 10.0) break;
    }
    
    if (d > 10.0) {
        col = vec3(0.0);
    } else {
        float theta = atan(p.x, p.z);
        float phi = acos(p.y / length(p));
        float wireDelta = PI / 18.0;
        float wireWidth = 0.02;
        float colTheta = smoothstep(wireWidth, 0.0, abs(mod(theta, wireDelta) - wireDelta/2.0));
        float colPhi = smoothstep(wireWidth, 0.0, abs(mod(phi, wireDelta) - wireDelta/2.0));
        col += vec3(max(colTheta, colPhi));
    }
    
    col = mix(5.5 * col, vec3(0.0), smoothstep(0.0, 3.6, depth));
    data_0[index] = packARGB(col);
}