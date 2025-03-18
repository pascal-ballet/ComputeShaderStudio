#define BLACK 0xFF000000
#define RED 0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE 0xFFFF0000
#define COLLISION_MARKER 0xFFFF00FF 
#define WHITE      0xFFFFFFFF
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

void circle(uint cx, uint cy, uint rayon, int epaisseur) {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    if ((x - cx) * (x - cx) + (y - cy) * (y - cy) >= (rayon - epaisseur) * (rayon - epaisseur) &&
        (x - cx) * (x - cx) + (y - cy) * (y - cy) <= rayon * rayon) {
            data_0[p] = RED;
    }
}

void circleFull(uint cx, uint cy, uint rayon, uint indice) {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    if(indice == 1){
        if ((x - cx) * (x - cx) + (y - cy) * (y - cy) <= rayon * rayon) {
            data_0[p] = BLUE; 
        }
    } else if(indice == 2){
        if ((x - cx) * (x - cx) + (y - cy) * (y - cy) <= rayon * rayon) {
            data_0[p] = RED; 
        } 
    }
}

bool checkCollision(uint cx, uint cy, uint radius, uint couleur) {
    for (int angle = 0; angle < 360; angle += 15) {
        float radians = float(angle) * 3.14159 / 180.0;
        uint checkX = cx + uint(float(radius) * cos(radians));
        uint checkY = cy + uint(float(radius) * sin(radians));
        
        if (checkX < WSX && checkY < WSY) {
            uint checkP = checkX + checkY * WSX;
            
            if (data_0[checkP] == couleur) {
                return true;
            }
        }
    }
    return false;
}

void drawSquare(uint centerX, uint centerY, uint size, uint couleur) {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    if (x >= centerX - size && x <= centerX + size &&
        y >= centerY - size && y <= centerY + size) {
            
            if (couleur == 1) {
                data_0[p] = DARK_GRAY;
            } else if (couleur == 2) {
                data_0[p] = YELLOW;
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


void drawCross(uint cx, uint cy, uint radius, uint thickness) {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    if ((x - cx) * (x - cx) + (y - cy) * (y - cy) >= (radius - thickness) * (radius - thickness) && 
        (x - cx) * (x - cx) + (y - cy) * (y - cy) <= radius * radius) {
        data_0[p] = WHITE;
    }

    if ((y >= cy - thickness && y <= cy + thickness && x >= cx - radius && x <= cx + radius) || 
        (x >= cx - thickness && x <= cx + thickness && y >= cy - radius && y <= cy + radius)) {
        data_0[p] = WHITE;
}
}

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
    bool hasCollided = (data_0[0] == COLLISION_MARKER);

    if (p != 0) {
        data_0[p] = BLACK;
    }

    drawSquare(1000, 50, 30, 1);
    drawCross(mousex, mousey, 50, 3);

    circle(540, 540, 200, 20);
    
    uint movingCircleRadius = 30;
    uint centerX = 540;
    
    float amplitude = 160.0;
    float frequency = 0.05;
    float sinValue = sin(float(step) * frequency); 
    uint normalY = uint(540.0 + amplitude * sinValue); 
    uint invertedY = uint(540.0 - amplitude * sinValue); 
    
   
    if (mouse_button > 0) {
       
        if ((mousex - centerX) * (mousex - centerX) + (mousey - normalY) * (mousey - normalY) <= movingCircleRadius * movingCircleRadius) {
            data_0[p] = CYAN;
        }
    }

    if (mouse_button > 0) {
        if (mousex >= 1000 - 30 && mousex <= 1000 + 30 &&
            mousey >= 50 - 30 && mousey <= 50 + 30) {
            
                for (uint i = 0; i < step +2; i++) {
                        uint size = step - (i * 10);
                        if (size > 0) {
                            // drawSquare(270, 270, size, (i + step / 30) % 15 + 1);
                            // drawSquare(810, 270, size, (i + step / 30) % 15 + 1);
                            // drawSquare(270, 810, size, (i + step / 30) % 15 + 1);
                            // drawSquare(810, 810, size, (i + step / 30) % 15 + 1);
                            drawSquare(540, 540, size, (i + step / 30) % 15 + 1);
                        }
                    }
        }
    }
    
    uint newcolor;
    if (!hasCollided) {
        newcolor = 2; 
        circleFull(centerX, normalY, movingCircleRadius, newcolor);
    } else {
        newcolor = 2;
        circleFull(centerX, invertedY, movingCircleRadius, newcolor);
    }
}

