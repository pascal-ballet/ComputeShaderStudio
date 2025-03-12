#define RADIUS 40

void main() {
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);

    // Fond initial en noir opaque
    if (step == 0) {
        data_0[p] = 0xFF000000 + y;
    } else {
        // Coordonnées relatives par rapport à la position de la souris (centre du cœur)
        float dx = float(x - mousex);
        float dy = float(y - mousey);
        
        // Variation dynamique de la taille du cœur pour un effet pulsant
        float r = RADIUS + 10.0 * sin(step / 10.0);
        
        // Mise à l'échelle pour adapter l'équation du cœur
        float sx = dx / r;
        float sy = dy / r;
        
        // Équation implicite du cœur : (x²+y²-1)³ - x²*y³ = 0
        float f = pow(sx * sx + sy * sy - 1.0, 3.0) - sx * sx * pow(sy, 3.0);
        
        // Utilisation de smoothstep pour adoucir la transition au niveau du contour
        // borderBlend passe de 0 (à l'intérieur) à 1 (à l'extérieur) sur une petite plage autour de f=0
        float borderBlend = smoothstep(-0.02, 0.02, f);
        // outsideBlend permet de faire disparaître progressivement les couleurs à l'extérieur du cœur
        float outsideBlend = smoothstep(0.02, 0.05, f);
        
        // Définition des couleurs (composantes entre 0 et 1)
        vec3 red   = vec3(1.0, 0.0, 0.0);
        vec3 blue  = vec3(0.0, 0.0, 1.0);
        vec3 black = vec3(0.0, 0.0, 0.0);
        
        // Mélange du rouge (intérieur) et du bleu (contour) selon borderBlend
        vec3 mixedColor = mix(red, blue, borderBlend);
        // Fait disparaître la couleur en fondu vers le noir à l'extérieur
        vec3 finalColor = mix(mixedColor, black, outsideBlend);
        
        // Optionnel : on peut rehausser légèrement la luminosité à l'intérieur du cœur
        if (f < -0.02) {
            finalColor += vec3(0.1);
            finalColor = clamp(finalColor, 0.0, 1.0);
        }
        
        // Conversion en couleur 32 bits 0xAARRGGBB
        int a     = 255;
        int rInt  = int(finalColor.r * 255.0);
        int gInt  = int(finalColor.g * 255.0);
        int bInt  = int(finalColor.b * 255.0);
        data_0[p] = (a << 24) + (rInt << 16) + (gInt << 8) + bInt;
    }
}
