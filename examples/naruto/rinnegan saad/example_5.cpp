void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    int dx = int(WSX) / 2 - int(x);
    int dy = int(WSY) / 2 - int(y);
    
    int d = dx * dx + dy * dy;

    // Condition : après 5 secondes (ajuste la valeur selon ton framerate)
    if (step > 300) { // Après 5 secondes -> Afficher les cercles

        float d = sqrt(float(dx * dx + dy * dy)); // Distance au centre

        // Animation des cercles (pulsation)
        float amplitude = 10.0;  // Amplitude de la pulsation
        float vitesse = 0.08;    // Vitesse de l'effet
        float animatedD = d + amplitude * sin(float(step) * vitesse);

        // Espacement animé des cercles
        float spacing = 30.0 + 10.0 * sin(float(step) * 0.1);

        // Calcul des cercles noirs avec un motif en anneaux
        float rings = mod(animatedD, spacing);
        bool isBlack = rings < 6.0; // Épaisseur des anneaux noirs

        if (isBlack) {
            data_0[p] = 0xFF000000; // Noir
        } else {
            data_0[p] = 0xFF800080; // Fond violet
        }

    } else {
        int value = clamp(600 * (d + int(step)), 0, 0xFFFFFF);
        data_0[p] = 0xFF000000 | (0xFFFFFF - value); // Rendu normal
    }
}

