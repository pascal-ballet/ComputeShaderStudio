void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y*WSX;
    
    // Normalize coordinates
    vec2 fragCoord = vec2(float(x), float(y));
    vec2 r = vec2(float(WSX), float(WSY));
    vec2 uv = fragCoord/r;
    vec2 centered = uv - 0.5;
    centered.x *= r.x/r.y;
    
    // Time control
    float t = float(step) * 0.03;
    
    // Simple black background
    vec3 finalColor = vec3(0.0, 0.0, 0.0);
    
    // Random lightning flashes
    for (int i = 0; i < 3; i++) {
        // Basic flickering effect
        float seed = float(i) * 3.14;
        float flickerSpeed = 0.5 + float(i) * 0.1;
        
        // Multi-frequency flickering
        float f1 = sin(t * flickerSpeed + seed * 13.37) * 0.5 + 0.5;
        float f2 = sin(t * flickerSpeed * 2.7 + seed * 4.53) * 0.5 + 0.5;
        float f3 = sin(t * flickerSpeed * 7.3 + seed * 27.71) * 0.5 + 0.5;
        float flashTiming = f1 * 0.6 + f2 * 0.3 + f3 * 0.1;
        
        // Only show lightning when above threshold
        float threshold = 0.7 + sin(t * 0.1 + seed) * 0.05;
        if (flashTiming > threshold) {
            // Calculate intensity
            float progress = (flashTiming - threshold) / (1.0 - threshold);
            float flashIntensity = pow(progress, 0.7) * 3.0;
            
            // Add micro-flickers
            float microFlicker = sin(t * 30.0 + seed * 5.0) * 0.5 + 0.5;
            flashIntensity = flashIntensity * (0.7 + 0.3 * microFlicker);
            
            // Random position that changes over time
            float posX = sin(t * 0.13 + seed * 12.34) * 0.8;
            float posY = cos(t * 0.11 + seed * 43.21) * 0.5;
            vec2 flashPos = vec2(posX, posY);
            
            // Distance to lightning center
            float dist = length(centered - flashPos);
            
            // Core and outer glow
            float baseRadius = 0.15 + sin(t * 4.0 + seed) * 0.02;
            float glowRadius = baseRadius * (1.0 + flashIntensity * 0.3);
            float coreSize = glowRadius * 0.3;
            
            float coreGlow = smoothstep(coreSize, 0.0, dist) * flashIntensity * 3.0;
            float outerGlow = smoothstep(glowRadius, coreSize, dist) * flashIntensity * 0.7;
            float glow = coreGlow + outerGlow;
            
            // Detail variation
            float detail = sin(dist * 20.0 + t * 5.0) * 0.5 + 0.5;
            glow = glow * (0.9 + detail * 0.1);
            
            // Color blend
            vec3 outerColor = vec3(0.85, 0.9, 1.0);
            vec3 coreColor = vec3(1.0, 1.0, 1.0);
            vec3 lightningColor = mix(outerColor, coreColor, min(1.0, coreGlow * 0.5));
            
            finalColor += lightningColor * glow;
        }
    }
    
    // Background ambient
    float bgGlow = sin(t * 0.2) * 0.5 + 0.5;
    bgGlow = pow(bgGlow, 5.0) * 0.05;
    finalColor += vec3(0.7, 0.8, 1.0) * bgGlow;
    
    // Convert to RGBA color format for data_0
    int red = int(min(finalColor.r * 255.0, 255.0));
    int green = int(min(finalColor.g * 255.0, 255.0));
    int blue = int(min(finalColor.b * 255.0, 255.0));
    
    data_0[p] = int(0xFF000000 | (red << 16) | (green << 8) | blue);
}