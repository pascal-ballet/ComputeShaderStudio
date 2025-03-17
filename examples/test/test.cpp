<<<<<<< HEAD
#define M_PI 3.14159265358979323846

#define blue1 vec3(0.74, 0.95, 1.00)
#define blue2 vec3(0.87, 0.98, 1.00)
#define blue3 vec3(0.35, 0.76, 0.83)
#define blue4 vec3(0.953, 0.969, 0.89)
#define red   vec3(1.00, 0.38, 0.227)

#define SMOOTH(r, R) (1.0 - smoothstep(R - 1.0, R + 1.0, r))
#define MOV(a, b, c, d, t) (vec2(a * cos(t) + b * cos(0.1 * (t)), c * sin(t) + d * cos(0.1 * (t))))

=======
>>>>>>> acdc12f316fab591711a0beaa2bfd7620111fab2
void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
<<<<<<< HEAD
    // Normalized coordinates
    vec2 uv = vec2(float(x), float(y));
    vec2 c = vec2(float(WSX) / 2.0, float(WSY) / 2.0); // Center
    
    // Scale factor to match the original effect size
    float scale = min(float(WSX), float(WSY)) / 650.0;
    
    // Time variable
    float time = float(step) * 0.01;
    
    // Start with a black background
    vec3 finalColor = vec3(0.0);
    
    // ---- Begin radar element calculations ----
    
    // Moving scan line
    {
        float theta0 = 90.0 * time;
        vec2 d = uv - c;
        float r = length(d);
        
        if (r < 240.0 * scale) {
            vec2 p = 240.0 * scale * vec2(cos(theta0 * M_PI / 180.0), -sin(theta0 * M_PI / 180.0));
            float l = length(d - p * clamp(dot(d, p) / dot(p, p), 0.0, 1.0));
            d = normalize(d);
            float theta = mod(180.0 * atan(d.y, d.x) / M_PI + theta0, 360.0);
            float gradient = clamp(1.0 - theta / 90.0, 0.0, 1.0);
            finalColor += (SMOOTH(l, 1.0) + 0.5 * gradient) * blue3;
        }
    }
    
    // Main circles
    {
        // Circle 1
        float r1 = length(uv - c);
        finalColor += (SMOOTH(r1 - 0.5 * scale, 100.0 * scale) - SMOOTH(r1 + 0.5 * scale, 100.0 * scale)) * blue1;
        
        // Circle 2
        float r2 = length(uv - c);
        finalColor += (SMOOTH(r2 - 0.5 * scale, 165.0 * scale) - SMOOTH(r2 + 0.5 * scale, 165.0 * scale)) * blue1;
        
        // Circle 3
        float r3 = length(uv - c);
        finalColor += (SMOOTH(r3 - 1.0 * scale, 240.0 * scale) - SMOOTH(r3 + 1.0 * scale, 240.0 * scale)) * blue4;
    }
    
    // Segmented outer circle
    {
        vec2 d = uv - c;
        float r = length(d);
        
        if (r > 0.0) {
            d = normalize(d);
            float theta = 180.0 * (atan(d.y, d.x) / M_PI);
            
            float segmentPattern = smoothstep(2.0, 2.1, abs(mod(theta + 2.0, 45.0) - 2.0)) *
                mix(0.5, 1.0, float(abs(mod(theta, 180.0) - 90.0) > 45.0));
                
            float circle = SMOOTH(r - 2.0 * scale, 313.0 * scale) - SMOOTH(r + 2.0 * scale, 313.0 * scale);
            
            finalColor += segmentPattern * circle * blue1;
        }
    }
    
    // Triangular elements
    {
        vec2 d = uv - c;
        float radius = (315.0 + 30.0 * sin(time)) * scale;
        
        // Right triangle
        float t1 = 0.0;
        if (d.x - radius >= -8.0 * scale && d.x - radius <= 0.0) {
            t1 = 1.0 - smoothstep(7.0 * scale + d.x - radius, 9.0 * scale + d.x - radius, abs(d.y));
        }
        
        // Left triangle
        float t2 = 0.0;
        if (d.x + radius >= 0.0 && d.x + radius <= 8.0 * scale) {
            t2 = 1.0 - smoothstep(7.0 * scale - d.x - radius, 9.0 * scale - d.x - radius, abs(d.y));
        }
        
        // Bottom triangle
        float t3 = 0.0;
        if (d.y - radius >= -8.0 * scale && d.y - radius <= 0.0) {
            t3 = 1.0 - smoothstep(7.0 * scale + d.y - radius, 9.0 * scale + d.y - radius, abs(d.x));
        }
        
        // Top triangle
        float t4 = 0.0;
        if (d.y + radius >= 0.0 && d.y + radius <= 8.0 * scale) {
            t4 = 1.0 - smoothstep(7.0 * scale - d.y - radius, 9.0 * scale - d.y - radius, abs(d.x));
        }
        
        float trianglePattern = t1 + t2 + t3 + t4;
        finalColor += trianglePattern * blue2;
    }
    
    // Center dot
    {
        float r = length(uv - c);
        finalColor += (SMOOTH(r - 0.5 * scale, 10.0 * scale) - SMOOTH(r + 0.5 * scale, 10.0 * scale)) * blue3;
    }
    
    // Semicircle with opening
    {
        vec2 d = uv - c;
        float r = length(d);
        
        if (r > 0.0) {
            d = normalize(d);
            float opening = 0.5 + 0.2 * cos(time);
            
            if (abs(d.y) > opening) {
                float circle = SMOOTH(r - 0.5 * scale, 262.0 * scale) - SMOOTH(r + 0.5 * scale, 262.0 * scale);
                finalColor += 0.7 * circle * blue3;
            }
        }
    }
    
    // Add blips if within radar range
    if (length(uv - c) < 240.0 * scale) {
        // Blip 1
        {
            vec2 p = 130.0 * scale * MOV(1.3, 1.0, 1.0, 1.4, 3.0 + 0.1 * time);
            float r = length(uv - (c + p));
            finalColor += SMOOTH(r, 3.0 * scale) * vec3(1, 1, 1);
        }
        
        // Blip 2
        {
            vec2 p = 130.0 * scale * MOV(0.9, -1.1, 1.7, 0.8, -2.0 + sin(0.1 * time) + 0.15 * time);
            float r = length(uv - (c + p));
            finalColor += SMOOTH(r, 3.0 * scale) * vec3(1, 1, 1);
        }
        
        // Blip 3 (animated)
        {
            vec2 p = 50.0 * scale * MOV(1.54, 1.7, 1.37, 1.8, sin(0.1 * time + 7.0) + 0.2 * time);
            float r = length(uv - (c + p));
            float R = (8.0 + mod(87.0 * time, 80.0)) * scale;
            
            float blip = (0.5 - 0.5 * cos(30.0 * time)) * SMOOTH(r, 5.0 * scale)
                + SMOOTH(6.0 * scale, r) - SMOOTH(8.0 * scale, r)
                + smoothstep(max(8.0 * scale, R - 20.0 * scale), R, r) - SMOOTH(R, r);
                
            finalColor += blip * red;
        }
    }
    
    // Add fixed targets with halos
    {
        vec2 targetPositions[3] = vec2[](
            c + vec2(0.3, 0.2) * float(WSX) * 0.5,
            c + vec2(-0.4, -0.3) * float(WSX) * 0.5,
            c + vec2(0.1, -0.5) * float(WSX) * 0.5
        );
        
        for (int i = 0; i < 3; i++) {
            float d = length(uv - targetPositions[i]);
            finalColor += red * exp(-50.0 * d / scale) * 0.7;
        }
    }
    
    // Convert to ARGB format
    int r = int(clamp(finalColor.r, 0.0, 1.0) * 255.0);
    int g = int(clamp(finalColor.g, 0.0, 1.0) * 255.0);
    int b = int(clamp(finalColor.b, 0.0, 1.0) * 255.0);
    int color = (r << 16) | (g << 8) | b;
    
    // Write to output
    data_0[p] = 0xFF000000 | color;
}
=======
    vec2 uv = vec2(float(x) / float(WSX), float(y) / float(WSY));
    vec2 pos = uv - 0.5;
    pos.x *= float(WSX) / float(WSY);
    
    float z = float(step) * 0.01;
    float l = length(pos);
    
    uv += pos / l * (sin(z) + 1.0) * abs(sin(l * 9.0 - z - z));
    
    // Adjusted colors for a more feminine palette
    float r = 0.12 / length(mod(uv + 0.4, vec2(1.0)) - 0.5);  // Soft pink
    float g = 0.05 / length(mod(uv + 0.6, vec2(1.0)) - 0.5);  // Light purple
    float b = 0.10 / length(mod(uv + 0.8, vec2(1.0)) - 0.5);  // Lavender
    
    int color = (int(r * 255.0) << 16) | (int(g * 255.0) << 8) | int(b * 255.0);
    data_0[p] = 0xFF000000 | color;
}


/// comm test
>>>>>>> acdc12f316fab591711a0beaa2bfd7620111fab2
