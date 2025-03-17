#define BLACK 0xFF000000
#define RED 0xFFFF0000
#define GREEN 0xFF00FF00
#define BLUE 0xFF0000FF

void circle(uint cx, uint cy, uint rayon, int epaisseur) {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    if ((x - cx) * (x - cx) + (y - cy) * (y - cy) >= (rayon - epaisseur) * (rayon - epaisseur) &&
        (x - cx) * (x - cx) + (y - cy) * (y - cy) <= rayon * rayon) {
            data_0[p] = RED;
    }
}

void circleFull1(uint cx, uint cy, uint rayon) {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    if ((x - cx) * (x - cx) + (y - cy) * (y - cy) <= rayon * rayon) {
        data_0[p] = BLUE; 
    }
}

void circleFull2(uint cx, uint cy, uint rayon) {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    if ((x - cx) * (x - cx) + (y - cy) * (y - cy) <= rayon * rayon) {
        data_0[p] = RED; 
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

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
    // Dessiner le cercle fixe
    circle(540, 540, 200, 10);
    
    uint movingCircleRadius = 30;
    
    // Mouvement en oscillation
    float amplitudeX = 100.0; // Amplitude du mouvement en X
    float amplitudeY = 150.0; // Amplitude du mouvement en Y
    float frequency = 0.01;   // Fréquence de l'oscillation

    uint centerX = uint(540.0 + amplitudeX * cos(float(step) * frequency));
    uint centerY = uint(540.0 + amplitudeY * sin(float(step) * frequency));

    bool collision = checkCollision(centerX, centerY, movingCircleRadius, RED);

    if (!collision) {
        circleFull1(centerX, centerY, movingCircleRadius);
    } else {
        circleFull2(centerX, centerY, movingCircleRadius);
    }
}
