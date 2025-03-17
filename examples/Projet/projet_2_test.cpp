#define BLACK      0xFF000000
#define WHITE      0xFFFFFFFF
#define RED        0xFF0000FF
#define GREEN      0xFF00FF00
#define BLUE       0xFFFF0000
#define YELLOW     0xFF00FFFF
#define CYAN       0xFFFFFF00
#define MAGENTA    0xFFFF00FF
#define ORANGE     0xFF0080FF
#define PURPLE     0xFF800080
#define PINK       0xFFFFAFAF
#define BROWN      0xFF4F2F0F
#define GRAY       0xFF808080
#define LIGHT_GRAY 0xFFC0C0C0
#define DARK_GRAY  0xFF404040

int x = int(gl_GlobalInvocationID.x);  // Conversion explicite en `int`
int y = int(gl_GlobalInvocationID.y);
int p = x + y * int(WSX);

void drawRotatedSquare(int centerX, int centerY, int size, int couleur, float angle) {
    float cosA = cos(angle);
    float sinA = sin(angle);

    for (int dx = -size; dx <= size; dx++) {
        for (int dy = -size; dy <= size; dy++) {
            int xOrig = centerX + dx;
            int yOrig = centerY + dy;

            // Rotation des coordonnées
            int xRot = centerX + int(dx * cosA - dy * sinA);
            int yRot = centerY + int(dx * sinA + dy * cosA);

            if (xRot >= 0 && xRot < int(WSX) && yRot >= 0 && yRot < int(WSY)) {
                int pRot = xRot + yRot * int(WSX);
                if (couleur == 1) {
                    data_0[pRot] = RED;
                } else if (couleur == 2) {
                    data_0[pRot] = BLUE;
                } else if (couleur == 3) {
                    data_0[pRot] = GREEN;
                } else if (couleur == 4) {
                    data_0[pRot] = BLACK;
                } else if (couleur == 5) {
                    data_0[pRot] = WHITE;
                } else if (couleur == 6) {
                    data_0[pRot] = YELLOW;
                } else if (couleur == 7) {
                    data_0[pRot] = CYAN;
                } else if (couleur == 8) {
                    data_0[pRot] = MAGENTA;
                } else if (couleur == 9) {
                    data_0[pRot] = ORANGE;
                } else if (couleur == 10) {
                    data_0[pRot] = PURPLE;
                } else if (couleur == 11) {
                    data_0[pRot] = PINK;
                } else if (couleur == 12) {
                    data_0[pRot] = BROWN;
                } else if (couleur == 13) {
                    data_0[pRot] = GRAY;
                } else if (couleur == 14) {
                    data_0[pRot] = LIGHT_GRAY;
                } else if (couleur == 15) {
                    data_0[pRot] = DARK_GRAY;
                }
            }
        }
    }
}

void main() {
    if (step == 0) {
        data_0[p] = GREEN; 
    }

    float angle = (step / 50)%360 * 3.14159 / 180.0; 
    for (int i = 0; i < step +2; i++) { 
        int size = step - (i * 10);
        if (size > 0) {

    drawRotatedSquare(540, 540, size,  (i + step / 30) % 15 + 1, angle);

        }
}
}




// #define BLACK      0xFF000000
// #define WHITE      0xFFFFFFFF
// #define RED        0xFF0000FF
// #define GREEN      0xFF00FF00
// #define BLUE       0xFFFF0000
// #define YELLOW     0xFF00FFFF
// #define CYAN       0xFFFFFF00
// #define MAGENTA    0xFFFF00FF
// #define ORANGE     0xFF0080FF
// #define PURPLE     0xFF800080
// #define PINK       0xFFFFAFAF
// #define BROWN      0xFF4F2F0F
// #define GRAY       0xFF808080
// #define LIGHT_GRAY 0xFFC0C0C0
// #define DARK_GRAY  0xFF404040

// int x = int(gl_GlobalInvocationID.x);  
// int y = int(gl_GlobalInvocationID.y);
// int p = x + y * int(WSX);

// // Matrice de rotation simplifiée
// void rotate(int inX, int inY, int centerX, int centerY, float cosA, float sinA, out int outX, out int outY) {
//     int relX = inX - centerX;
//     int relY = inY - centerY;

//     outX = centerX + int(relX * cosA - relY * sinA);
//     outY = centerY + int(relX * sinA + relY * cosA);
// }

// void drawRotatedSquare(int centerX, int centerY, int size, int couleur, float angle) {
//     float cosA = cos(angle);
//     float sinA = sin(angle);

//     int x1, y1, x2, y2, x3, y3, x4, y4;

//     // Calcul des 4 coins du carré
//     rotate(centerX - size, centerY - size, centerX, centerY, cosA, sinA, x1, y1);
//     rotate(centerX + size, centerY - size, centerX, centerY, cosA, sinA, x2, y2);
//     rotate(centerX - size, centerY + size, centerX, centerY, cosA, sinA, x3, y3);
//     rotate(centerX + size, centerY + size, centerX, centerY, cosA, sinA, x4, y4);

//     // On ne modifie que les bords du carré (pas l'intérieur)
//     if ((x >= x1 && x <= x2 && (y == y1 || y == y3)) || 
//         (y >= y1 && y <= y3 && (x == x1 || x == x2))) {

//         int pRot = x + y * int(WSX);

//         // Choix de la couleur avec `if` 
//         if (couleur == 1) {
//             data_0[pRot] = RED;
//         } else if (couleur == 2) {
//             data_0[pRot] = BLUE;
//         } else if (couleur == 3) {
//             data_0[pRot] = GREEN;
//         } else if (couleur == 4) {
//             data_0[pRot] = BLACK;
//         } else if (couleur == 5) {
//             data_0[pRot] = WHITE;
//         } else if (couleur == 6) {
//             data_0[pRot] = YELLOW;
//         } else if (couleur == 7) {
//             data_0[pRot] = CYAN;
//         } else if (couleur == 8) {
//             data_0[pRot] = MAGENTA;
//         } else if (couleur == 9) {
//             data_0[pRot] = ORANGE;
//         } else if (couleur == 10) {
//             data_0[pRot] = PURPLE;
//         } else if (couleur == 11) {
//             data_0[pRot] = PINK;
//         } else if (couleur == 12) {
//             data_0[pRot] = BROWN;
//         } else if (couleur == 13) {
//             data_0[pRot] = GRAY;
//         } else if (couleur == 14) {
//             data_0[pRot] = LIGHT_GRAY;
//         } else if (couleur == 15) {
//             data_0[pRot] = DARK_GRAY;
//         }
//     }
// }

// void main() {
//     if (step == 0) {
//         data_0[p] = GREEN; 
//     }

//     // Réduction de la vitesse de rotation
//     float angle = (step / 20.0) * 3.14159 / 180.0;  

//     for (int i = 0; i < step +2; i++) { 
//         int size = step - (i * 10);
//         if (size > 0) {
//             drawRotatedSquare(540, 540, size, (i + step / 30) % 15 + 1, angle);
//         }
//     }
// }
