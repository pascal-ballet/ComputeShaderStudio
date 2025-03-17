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
    
    // Convert to RGBA color format for data_0
    int red = int(min(finalColor.r * 255.0, 255.0));
    int green = int(min(finalColor.g * 255.0, 255.0));
    int blue = int(min(finalColor.b * 255.0, 255.0));
    
    data_0[p] = int(0xFF000000 | (red << 16) | (green << 8) | blue);
}