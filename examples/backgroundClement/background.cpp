float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// inspiré par le background shadertoy suivant : https://www.shadertoy.com/view/Wf2GDt
float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    
    // Quatre coins du carré
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    
    // Interpolation lisse
    vec2 u = f * f * (3.0 - 2.0 * f);
    
    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
    //variable time basé sur la variable step de computeShaderStudio 
    float iTime = step / 10.0;
    vec2 fragCoord = vec2(float(x), float(y));
    vec2 iResolution = vec2(float(WSX), float(WSY));
    vec4 fragColor = vec4(0.0);
    
    // Conversion du code shadertoy
    float t = iTime / 10.0;
    
    vec2 pos = 100.0 * fragCoord/(iResolution.xy);
    float yPos = pos.y/100.0;
    pos.y += sin(2.0*t);
    pos.x += cos(5.0*t);
    pos *= mat2(sin(t/2.0),-cos(t/2.0),cos(t/2.0),sin(t/2.0))/8.0;
    
    // Boucle de l'effet de fractale
    for (float i = 0.0; i < 8.0; i++) {
        fragColor = cos(pos.xxxx*0.3)*0.5+0.5;
        float n = noise(pos/5.0);
        fragColor *= n;
        pos.x += sin(pos.y + iTime*0.3 + i);
        pos *= mat2(6.0,-8.0,8.0,6.0)/8.0;
    }
    
    // Fade vertical
    fragColor *= 1.0 - smoothstep(0.0, 0.91, yPos);
    
    // Teinte rouge
    fragColor *= vec4(0.2, 0.23, 0.54, 1.0);
    
    // Conversion de la couleur au format requis par le buffer
    uint r = uint(min(255.0, fragColor.r * 255.0));
    uint g = uint(min(255.0, fragColor.g * 255.0));
    uint b = uint(min(255.0, fragColor.b * 255.0));
    uint alpha = 0xFF;
    
    // Construction de la couleur finale
    uvec4 color = uvec4((alpha << 24) | (r << 16) | (g << 8) | b);
    
    // Écriture dans le buffer
    data_0[p] = int(color.x);
}