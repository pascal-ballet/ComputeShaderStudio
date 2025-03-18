// Définition des couleurs
#define BLACK      0xFF000000
#define WHITE      0xFFFFFFFF
#define RED        0xFF0000FF
#define GREEN      0xFF00FF00
#define BLUE       0xFFFF0000
#define YELLOW     0xFF00FFFF
#define CYAN       0xFFFFFF00
#define MAGENTA    0xFFFF00FF

// Variables globales pour suivre l'état des carrés
bool isErased1 = false;
bool isErased2 = false;
bool isErased3 = false;
bool isErased4 = false;
bool isErased5 = false;  // Suivre si le premier carré est effacé
// Vous pouvez créer de nombreuses variables comme celles-ci pour d'autres carrés si nécessaire

uint x = gl_GlobalInvocationID.x;
uint y = gl_GlobalInvocationID.y;
uint p = x + y * WSX;

// Fonction de hachage pour la génération aléatoire
uint wang_hash(uint seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

// Fonction pour dessiner un carré
void drawSquare(uint centerX, uint centerY, uint size, uint couleur) {
    if (x >= centerX - size && x <= centerX + size &&
        y >= centerY - size && y <= centerY + size) {
        
        if (couleur == 1) {
            data_0[p] = RED;
        } else if (couleur == 2) {
            data_0[p] = BLUE;
        } else if (couleur == 3) {
            data_0[p] = GREEN;
        } else if (couleur == 4) {
            data_0[p] = YELLOW;
        } else if (couleur == 5) {
            data_0[p] = CYAN;
        } else if (couleur == 6) {
            data_0[p] = MAGENTA;
        }
    }
}

// Fonction principale
void main() {
    // Remplir l'écran en noir au premier frame
    if (step == 0) {
        data_0[p] = BLACK;

        
    }

    
        uint seed = 12345;

        uint centerX1 = 100 + wang_hash(seed + step) % 880;
        uint centerY1 = 100 + wang_hash(seed + step) % 880;
        uint size1 = 20 + wang_hash(seed + step) % 60;
        uint color1 = 1 + wang_hash(seed + step) % 6;

        uint centerX2 = 100 + wang_hash(seed+ step + 50) % 880;
    uint centerY2 = 100 + wang_hash(seed + step+ 60) % 880;
    uint size2 = 20 + wang_hash(seed + step+ 70) % 60;
    uint color2 = 1 + wang_hash(seed + step+ 80) % 6;
    
    uint centerX3 = 100 + wang_hash(seed+ step + 90) % 880;
    uint centerY3 = 100 + wang_hash(seed + step+ 100) % 880;
    uint size3 = 20 + wang_hash(seed+ step + 110) % 60;
    uint color3 = 1 + wang_hash(seed + step+ 120) % 6;
    
    uint centerX4 = 100 + wang_hash(seed + step+ 130) % 880;
    uint centerY4 = 100 + wang_hash(seed + step+ 140) % 880;
    uint size4 = 20 + wang_hash(seed + step+ 150) % 60;
    uint color4 = 1 + wang_hash(seed + step+ 160) % 6;
    
    uint centerX5 = 100 + wang_hash(seed+ step + 170) % 880;
    uint centerY5 = 100 + wang_hash(seed + step+ 180) % 880;
    uint size5 = 20 + wang_hash(seed+ step + 190) % 60;
    uint color5 = 1 + wang_hash(seed + step+ 200) % 6;

        
    
    drawSquare(1000, 50, 30,1);
    
    // Utiliser une graine fixe pour avoir des positions stables
    

    // Coordonnées et taille du premier carré
    

    // Si le carré n'a pas été effacé, dessiner
    if (step == 1) {
        drawSquare(centerX1, centerY1, size1, color1);
        drawSquare(centerX2, centerY2, size2, color2);
        drawSquare(centerX3, centerY3, size3, color3);
        drawSquare(centerX4, centerY4, size4, color4);
        drawSquare(centerX5, centerY5, size5, color5);
    }

    if (mouse_button > 0) {
        if (mousex >= 1000 - 30 && mousex <= 1000 + 30 &&
            mousey >= 50 - 30 && mousey <= 50 + 30) {
            
            drawSquare(centerX1, centerY1, size1, color1);
        }
    }


    // Vérification si la souris est dans la zone du carré
    if (mouse_button > 0) {
        if (mousex >= centerX1 - size1 && mousex <= centerX1 + size1 &&
            mousey >= centerY1 - size1 && mousey <= centerY1 + size1) {
            
            // Effacer le carré en changeant la couleur à noir
            data_0[p] = BLACK;
            isErased1 = true;  // Marquer l'état comme effacé
        }else if (mousex >= centerX2 - size2 && mousex <= centerX2 + size2 &&
            mousey >= centerX2 - size2 && mousey <= centerX2 + size2) {
            
            // Effacer le carré en changeant la couleur à noir
            data_0[p] = BLACK;
            isErased2 = true;  // Marquer l'état comme effacé
        }
    }
}
