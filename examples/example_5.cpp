void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    int dx = int(WSX) / 2 - int(x);
    int dy = int(WSY) / 2 - int(y);
    
    int d = dx * dx + dy * dy;
    int r = int(64.0 * sin(float(step) / 100.0));

    // Condition : après 5 secondes (ajuste la valeur selon ton framerate)
    if (step > 300) { // Après 5 secondes -> Afficher les cercles

        int d = int(sqrt(float(dx * dx + dy * dy))); // Distance au centre
        int spacing = 30; // Espacement des cercles (réduit)
        
        // Calcul des cercles noirs avec un motif en anneaux
        float rings = mod(float(d), float(spacing));
        bool isBlack = rings < 6.0; // Réduction de l'épaisseur des anneaux noirs

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
