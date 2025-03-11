void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y*WSX;

    // Normalize coordinates
    vec2 fragCoord = vec2(float(x), float(y));
    vec2 r = vec2(float(WSX), float(WSY));
    
    // Calculate time - using step as time
    float t = float(step) * 0.05;
    
    vec3 c;
    float l;
    float z = t;
    
    for(int i=0; i<3; i++) {
        vec2 uv, p = fragCoord/r;
        uv = p;
        p -= 0.5;
        p.x *= r.x/r.y;
        z += 0.07;
        l = length(p);
        
        // Adding mouse interaction to the original shader
        float mouseInfluence = 0.0;
        if (mousex > 0 && mousey > 0) {
            vec2 mousePos = vec2(float(mousex)/r.x, float(mousey)/r.y) - 0.5;
            mousePos.x *= r.x/r.y;
            float mouseDist = length(p - mousePos);
            mouseInfluence = 0.5 * exp(-mouseDist * 4.0);
        }
        
        uv += p/l * (sin(z + mouseInfluence) + 1.0) * abs(sin(l * 9.0 - z - z));
        c[i] = 0.01/length(mod(uv, 1.0) - 0.5);
    }
    
    // Convert to RGBA color format for data_0
    int red = int(min(c.r * 255.0, 255.0));
    int green = int(min(c.g * 255.0, 255.0));
    int blue = int(min(c.b * 255.0, 255.0));
    
    data_0[p] = int(0xFF000000 | (red << 16) | (green << 8) | blue);
}