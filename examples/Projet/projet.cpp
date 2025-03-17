#define BLACK 0xFF000000
#define RED 0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE 0xFFFF0000

#define RAYON 100

void circle(uint cx, uint cy, uint rayon, int epaisseur) {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    if ((x - cx) * (x - cx) + (y - cy) * (y - cy) >= (rayon - epaisseur) * (rayon - epaisseur) &&
        (x - cx) * (x - cx) + (y - cy) * (y - cy) <= rayon * rayon) {
            data_0[p] = RED;
    }

    

    
}

// void TEST(uint cx, uint cy, uint rayon, int epaisseur) {
//     uint x = gl_GlobalInvocationID.x;
//     uint y = gl_GlobalInvocationID.y;
//     uint p = x + y * WSX;

//     if ((x - cx) * (x - cx) + (y - cy) * (y - cy) >= (rayon - epaisseur) * (rayon - epaisseur) &&
//         (x - cx) * (x - cx) + (y - cy) * (y - cy) <= rayon * rayon) {
//             data_0[p] = RED;
//     }

    

    
// }

void circleFull(uint cx, uint cy, uint rayon) {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

   
    if ((x - cx) * (x - cx) + (y - cy) * (y - cy) <= rayon * rayon) {
        data_0[p] = BLUE; 
    }
}

// bool checkCollision(uint cx, uint cy) {
//     for (int i = -1; i <= 1; i++) {
//         for (int j = -1; j <= 1; j++) {
//             uint nx = cx + i;  
//             uint ny = cy + j; 
//             uint p = nx + ny * WSX;
            
//             if (data_0[p] == RED) {
//                 return true; 
//             }
//         }
//     }
//     return false;
// }

// void main() {
//     uint x = gl_GlobalInvocationID.x;
//     uint y = gl_GlobalInvocationID.y;
//     uint p = x + y * WSX;
    
//     circle(540, 540, 200, 10);
    
//     bool collision = checkCollision(540, step+540);
    
//     if(!collision) {
//         circleFull(540, step+540, 30);
//     } else {
//         TEST(10, 10, 200, 10);
//         //circleFull(540, -step, 30);
//     }
// }


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
    
    circle(540, 540, 200, 10);
    
    uint movingCircleRadius = 30;
    uint centerX = 540;
    uint centerY = step + 540;
    
    bool collision = checkCollision(centerX, centerY, movingCircleRadius, RED);
    
    if (!collision) {
        circleFull(centerX, centerY, movingCircleRadius);
    } else {
        circleFull(centerX, (step*-1)+540, movingCircleRadius);
    }
}





































// if (data_0[p] != 0xFFFFFFFF && data_0[p] != 0x00000000) {
    //     if (data_0[p + WSX] == 0x00000000) {
    //         data_0[p + WSX] = data_0[p];
    //         data_0[p] = 0x00000000;
    //     }
    // }
    //circle(mousex,mousey,300);
    // circle(540,540,200,10);
    // if(data_0[p] != RED){
    //     circleFull(540,540,step);

    // }else{
    // circleFull(540,540, 199);
    // }

    // circle(540,540,200,10);
    // circleFull(540,540,100);

    // if (x == mousex && y == mousey) {
    //     data_0[p] = BLUE;
    // }

    // // si le pixel est non vide et non sol
    // if (data_0[p] != 0xFFFFFFFF && data_0[p] != 0x00000000) {
    //     // il tombe
    //     if (data_0[p + WSX] == 0x00000000) {
    //         data_0[p + WSX] = data_0[p];
    //         data_0[p] = 0x00000000;
    //     }
    // }



 // uint x = gl_GlobalInvocationID.x;
    // uint y = gl_GlobalInvocationID.y;
    // uint p = x + y * WSX;


    
    // data_0[p] = 0xFFFFFFFF; 

    // if((x < 32 && x > 10) && y<32&&y>10)data_0[p] = RED;

    

    // if (!checkCollision(540, 540, step)) {
    //     circleFull(540, 540, step);
    // } else {
       
    //     circleFull(540, 540, -step); // Fixe le dernier cercle avant la collision
    // }

    // // Cercle rouge fixe au centre
    // circle(540, 540, 200, 10);