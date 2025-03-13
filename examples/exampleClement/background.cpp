// Write your code HERE
void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
    // Couleur par défaut (noir)
    uvec4 color = uvec4(0xFF000000);
    
    // Créer un quadrillage avec effet de perspective dans la moitié inférieure
    if (y >= WSY / 2) {
        // Point de fuite au centre horizontal, en haut de la zone du quadrillage
        float vanishX = WSX / 2.0;
        float vanishY = WSY / 2.0;
        
        // Calculer les coordonnées relatives au point de fuite
        float dx = float(x) - vanishX;
        float dy = float(y) - vanishY;
        
        // Calculer la distance par rapport au point de fuite
        float dist = sqrt(dx*dx + dy*dy);
        
        // Créer les lignes horizontales (parallèles à l'axe X)
        float lineSpacing = 20.0;
        float distMod = mod(dist, lineSpacing);
        
        // Créer les lignes radiales (partant du point de fuite)
        float radialSpacing = 3.14159 / 15.0; // Environ 12 degrés entre les lignes
        float angle = atan(dx, dy); // En radians
        float angleMod = mod(angle, radialSpacing);
        
        // Dessiner les lignes avec une épaisseur qui diminue avec la distance
        float lineThickness = max(1.0, 3.0 - dist / 100.0);
        
        if (distMod < lineThickness || angleMod < 0.02 || angleMod > radialSpacing - 0.02) {
            // Couleur rose fluo pour un effet futuristique
            uint alpha = 0xFF;
            uint intensity = 0xFF00FF; // Rose fluo
            color = uvec4((alpha << 24) | intensity);
        }
    }
    
    // Conversion explicite en int pour résoudre l'erreur de type
    data_0[p] = int(color.x);
}
