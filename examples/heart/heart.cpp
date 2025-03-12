
#define POINT_COUNT 8
const float speed = -0.5;
const float len = 0.25;
const float scale = 0.012;
float intensity = 1.3;
float radius = 0.015;

// Signed distance to a quadratic bezier
float sdBezier(vec2 pos, vec2 A, vec2 B, vec2 C) {    
    vec2 a = B - A;
    vec2 b = A - 2.0*B + C;
    vec2 c = a * 2.0;
    vec2 d = A - pos;
    float kk = 1.0 / dot(b,b);
    float kx = kk * dot(a,b);
    float ky = kk * (2.0*dot(a,a)+dot(d,b)) / 3.0;
    float kz = kk * dot(d,a);      
    float res = 0.0;
    float p = ky - kx*kx;
    float p3 = p*p*p;
    float q = kx*(2.0*kx*kx - 3.0*ky) + kz;
    float h = q*q + 4.0*p3;
    if(h >= 0.0){ 
        h = sqrt(h);
        vec2 x = (vec2(h, -h) - q) / 2.0;
        vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
        float t = uv.x + uv.y - kx;
        t = clamp( t, 0.0, 1.0 );
        // 1 root
        vec2 qos = d + (c + b*t)*t;
        res = length(qos);
    } else {
        float z = sqrt(-p);
        float v = acos( q/(p*z*2.0) ) / 3.0;
        float m = cos(v);
        float n = sin(v)*1.732050808;
        vec3 t = vec3(m + m, -n - m, n - m) * z - kx;
        t = clamp( t, 0.0, 1.0 );
        // 3 roots
        vec2 qos = d + (c + b*t.x)*t.x;
        float dis = dot(qos,qos);
        
        res = dis;
        qos = d + (c + b*t.y)*t.y;
        dis = dot(qos,qos);
        res = min(res,dis);
        qos = d + (c + b*t.z)*t.z;
        dis = dot(qos,qos);
        res = min(res,dis);
        res = sqrt( res );
    }
    
    return res;
}

// Function to get the heart position
vec2 getHeartPosition(float t) {
    return vec2(16.0 * sin(t) * sin(t) * sin(t),
                -(13.0 * cos(t) - 5.0 * cos(2.0*t)
                - 2.0 * cos(3.0*t) - cos(4.0*t)));
}

// Function to get the glow
float getGlow(float dist, float radius, float intensity) {
    return pow(radius/dist, intensity);
}

float getSegment(float t, vec2 pos, float offset) {
    vec2 points[POINT_COUNT];
    for(int i = 0; i < POINT_COUNT; i++) {
        points[i] = getHeartPosition(offset + float(i)*len + fract(speed * t) * 6.28);
    }
    
    vec2 c = (points[0] + points[1]) / 2.0;
    vec2 c_prev;
    float dist = 10000.0;
    
    for(int i = 0; i < POINT_COUNT-1; i++) {
        c_prev = c;
        c = (points[i] + points[i+1]) / 2.0;
        dist = min(dist, sdBezier(pos, scale * c_prev, scale * points[i], scale * c));
    }
    return max(0.0, dist);
}

void main() {
    // Get pixel coordinates
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    vec2 fragCoord = vec2(pixelCoords);
    
    // Check if we're within the image bounds
    if (pixelCoords.x >= int(iResolution.x) || pixelCoords.y >= int(iResolution.y)) {
        return;
    }
    
    // Execute the shader
    vec2 uv = fragCoord / iResolution.xy;
    float widthHeightRatio = iResolution.x / iResolution.y;
    vec2 centre = vec2(0.5, 0.5);
    vec2 pos = centre - uv;
    pos.y /= widthHeightRatio;
    
    // Shift upwards to center the heart
    pos.y += 0.03;
    
    float t = iTime;
    
    // Get first segment
    float dist = getSegment(t, pos, 0.0);
    float glow = getGlow(dist, radius, intensity);
    
    vec3 col = vec3(0.0);
    
    // White core
    col += 10.0 * vec3(smoothstep(0.006, 0.003, dist));
    // Pink glow
    col += glow * vec3(1.0, 0.05, 0.3);
    
    // Get second segment
    dist = getSegment(t, pos, 3.4);
    glow = getGlow(dist, radius, intensity);
    
    // White core
    col += 10.0 * vec3(smoothstep(0.006, 0.003, dist));
    // Blue glow
    col += glow * vec3(0.1, 0.4, 1.0);
    
    // Tone mapping
    col = 1.0 - exp(-col);
    
    // Gamma
    col = pow(col, vec3(0.4545));
    
    // Write to output image
    imageStore(outputTexture, pixelCoords, vec4(col, 1.0));
}