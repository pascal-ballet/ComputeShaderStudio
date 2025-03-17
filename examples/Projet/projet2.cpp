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

uint x = gl_GlobalInvocationID.x;
uint y = gl_GlobalInvocationID.y;
uint p = x + y * WSX;

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
                data_0[p] = BLACK;
            } else if (couleur == 5) {
                data_0[p] = WHITE;
            } else if (couleur == 6) {
                data_0[p] = YELLOW;
            } else if (couleur == 7) {
                data_0[p] = CYAN;
            } else if (couleur == 8) {
                data_0[p] = MAGENTA;
            } else if (couleur == 9) {
                data_0[p] = ORANGE;
            } else if (couleur == 10) {
                data_0[p] = PURPLE;
            } else if (couleur == 11) {
                data_0[p] = PINK;
            } else if (couleur == 12) {
                data_0[p] = BROWN;
            } else if (couleur == 13) {
                data_0[p] = GRAY;
            } else if (couleur == 14) {
                data_0[p] = LIGHT_GRAY;
            } else if (couleur == 15) {
                data_0[p] = DARK_GRAY;
            }
            
            
        
        
    }
}


void circle(uint cx, uint cy, uint rayon, int epaisseur) {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    if ((x - cx) * (x - cx) + (y - cy) * (y - cy) >= (rayon - epaisseur) * (rayon - epaisseur) &&
        (x - cx) * (x - cx) + (y - cy) * (y - cy) <= rayon * rayon) {
            data_0[p] = RED;
    }
}

void main() {
    if (step == 0) {
        data_0[p] = GREEN;
    }
    uint centerX = 540;
    uint centerY = 540;
    // for (uint i = 0; i < step +2; i++) {
    //     uint size = step - (i * 10);
    //     if (size > 0) {
    //         // drawSquare(270, 270, size, (i + step / 30) % 15 + 1);
    //         // drawSquare(810, 270, size, (i + step / 30) % 15 + 1);
    //         // drawSquare(270, 810, size, (i + step / 30) % 15 + 1);
    //         // drawSquare(810, 810, size, (i + step / 30) % 15 + 1);
    //         drawSquare(540, 540, size, (i + step / 30) % 15 + 1);
    //     }
    // }

    //drawSquare(540, 540, 20, 1);
        //circle(mousex, mousey, 50, 3);
    
        if ((mouse_button)> 0) {
            circle(mousex, mousey, 50, 3);
        }
}
