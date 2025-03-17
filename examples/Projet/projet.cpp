#define BLACK 0xFF000000
#define RED 0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE 0xFFFF0000
#define COLLISION_MARKER 0xFFFF00FF 

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

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
    bool hasCollided = (data_0[0] == COLLISION_MARKER);
    
    if (p != 0) {
        data_0[p] = BLACK;
    }
    
    circle(540, 540, 200, 20);
    
    uint movingCircleRadius = 30;
    uint centerX = 540;
    
    float amplitude = 160.0;
    float frequency = 0.01; 
    uint normalY = uint(540.0 + amplitude * sin(float(step) * frequency));
    uint invertedY = uint(540.0 - amplitude * sin(float(step) * frequency));
    
    if (!hasCollided) {
        bool collision = checkCollision(centerX, normalY, movingCircleRadius, RED);
        if (collision) {
            data_0[0] = COLLISION_MARKER;
            hasCollided = true;
        }
    }
    
    uint newcolor;
    
    if (!hasCollided) {
        newcolor = 1; // BLUE
        circleFull(centerX, normalY, movingCircleRadius, newcolor);
    } else {
        newcolor = 2; // RED
        circleFull(centerX, invertedY, movingCircleRadius, newcolor);
    }
}