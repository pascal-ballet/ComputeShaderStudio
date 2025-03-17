void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y*WSX;
    
    vec2 fragCoord = vec2(float(x), float(y));
    vec2 r = vec2(float(WSX), float(WSY));
    vec2 uv = fragCoord/r;
    vec2 centered = uv - 0.5;
    centered.x *= r.x/r.y;
    
    float t = float(step) * 0.03;
    
    //fond noir simple
    vec3 finalColor = vec3(0.0, 0.0, 0.0);
    
    //flash de lumière random 
    for (int i = 0; i < 3; i++) {
        
        float flashTiming = sin(t * (0.2 + float(i) * 0.07) + float(i) * 5.73) * 0.5 + 0.5;
        
        if (flashTiming > 0.8) {

            float flashIntensity = (flashTiming - 0.8) * 5.0; // 0-1 range
 
            float posX = sin(t * 0.13 + float(i) * 12.34) * 0.8;
            float posY = cos(t * 0.11 + float(i) * 43.21) * 0.5;
            vec2 flashPos = vec2(posX, posY);
            
            float dist = length(centered - flashPos);

            float glowRadius = 0.15 + sin(t * 10.0 + float(i)) * 0.05;
            float glow = smoothstep(glowRadius, 0.0, dist) * flashIntensity;
            
            vec3 lightningColor = vec3(0.9, 0.95, 1.0);
            finalColor += lightningColor * glow;
        }
    }

    int red = int(min(finalColor.r * 255.0, 255.0));
    int green = int(min(finalColor.g * 255.0, 255.0));
    int blue = int(min(finalColor.b * 255.0, 255.0));
    
    data_0[p] = int(0xFF000000 | (red << 16) | (green << 8) | blue);
}