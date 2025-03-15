void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
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