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
        
        // Créer les lignes horizontales (parallèles à l'axe X) - mailles plus fines
        float lineSpacing = 15.0; // Espacement réduit pour des mailles plus fines
        float distMod = mod(dist, lineSpacing);
        
        // Créer les lignes radiales (partant du point de fuite) - plus nombreuses
        float radialSpacing = 3.14159 / 36.0; // Doublement du nombre de lignes radiales
        float angle = atan(dx, dy); // En radians
        float angleMod = mod(angle, radialSpacing);
        
        // Anti-aliasing amélioré pour les lignes
        // Largeur de ligne qui diminue avec la distance
        float baseThickness = 1.0; // Ligne plus fine
        float lineThickness = max(0.3, baseThickness - dist / 400.0);
        
        // Calculer l'intensité pour un effet de dégradé (anti-aliasing amélioré)
        float lineIntensity = 0.0;
        float radialIntensity = 0.0;
        
        // Anti-aliasing doux pour les lignes horizontales
        float fadeWidth = 1.0; // Zone de transition adaptée aux lignes plus fines
        if (distMod < lineThickness) {
            lineIntensity = 1.0;
        } else if (distMod < lineThickness + fadeWidth) {
            // Fonction de lissage cubique pour une transition plus douce
            float t = (distMod - lineThickness) / fadeWidth;
            lineIntensity = 1.0 - (t * t * (3.0 - 2.0 * t));
        }
        
        // Anti-aliasing doux pour les lignes radiales
        float radialEdge = 0.03 + 0.01 * (dist / 300.0); // Zone de transition adaptée
        if (angleMod < radialEdge) {
            // Transition douce près du bord
            float t = angleMod / radialEdge;
            radialIntensity = 1.0 - (t * t * (3.0 - 2.0 * t));
        } else if (angleMod > radialSpacing - radialEdge) {
            // Transition douce près du bord opposé
            float t = (radialSpacing - angleMod) / radialEdge;
            radialIntensity = 1.0 - (t * t * (3.0 - 2.0 * t));
        }
        
        // Utiliser l'intensité maximale entre les deux types de lignes
        float finalIntensity = max(lineIntensity, radialIntensity);
        
        if (finalIntensity > 0.0) {
            // Couleur rose fluo pour un effet futuristique, avec intensité variable
            uint alpha = 0xFF;
            uint r = uint(min(255.0, 230.0 * finalIntensity));
            uint b = uint(min(255.0, 255.0 * finalIntensity));
            uint g = uint(min(255.0, 50.0 * finalIntensity)); // Un peu de vert pour plus de profondeur
            color = uvec4((alpha << 24) | (r << 16) | (g << 8) | b);
        }
    }
    
    // Conversion explicite en int pour résoudre l'erreur de type
    data_0[p] = int(color.x);
}