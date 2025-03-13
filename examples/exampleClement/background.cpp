// Write your code HERE
void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
    // Couleur par défaut (noir)
    uvec4 color = uvec4(0xFF000000);
    
    // Créer un quadrillage uniquement dans la moitié inférieure de l'écran
    if (y >= WSY / 2) {
        // Taille des cellules du quadrillage
        uint gridSize = 20;
        
        // Si on est sur une ligne de la grille
        if (x % gridSize == 0 || y % gridSize == 0) {
            // Couleur rose fluo pour un effet futuristique
            color = uvec4(0xFFFF00FF); // Format ARGB: Rose fluo (FF magenta)
        }
    }
    
    // Conversion explicite en int pour résoudre l'erreur de type
    data_0[p] = int(color.x);
}
