#define CERCLE 0xFFFFFFFF
#define FOND 0x00000000
#define RAYON 50.0
#define PI 3.141592

void cercle(uint cy, uint cx) {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    float distance = sqrt(float((x - cx) * (x - cx) + (y - cy) * (y - cy)));
    
    if (distance <= RAYON) {
        data_0[p] = CERCLE;
    } else {
        data_0[p] = FOND;
    }
}

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    //nettoyage
    data_0[p] = FOND;

    //pos souris
    uint mouse_x = uint(mousex);
    uint mouse_y = uint(mousey);

    //etat souris
    bool mouse_pressed = mouse_button > 0;

    //stocke coordonnees de lancement
    bool depart_balle = (data_1[0] == 1);
    uint depart_x = uint(data_1[1]);
    uint depart_y = uint(data_1[2]);
    uint depart_step = uint(data_1[3]);

    if (mouse_pressed && (!depart_balle || (mouse_x != depart_x || mouse_y != depart_y))) {
        //etat lance
        data_1[0] = 1;
        
        //Enregistre lancement
        data_1[1] = int(mouse_x);
        data_1[2] = int(mouse_y); 
        data_1[3] = int(step);
        
        //Maj var locales
        depart_balle = true;
        depart_x = mouse_x;
        depart_y = mouse_y;
        depart_step = uint(step);
    }
    
    
    //calc pos
    if (depart_balle) {
        float temps_ecoule = float(step - depart_step);
        float maxHauteur = float(depart_y);
        float minHauteur = float(WSY - uint(RAYON));
        float tempsRebond = 90.0;
        float energiePerdue = 0.85;
        
        // Position calculation: start at bottom, bounce up with decreasing height
        //float y_pos = maxHeight - amplitude * (0.5 + 0.5 * sin(phase - PI/2.0));
        

        float pos_y;
        if (temps_ecoule < 1.0) {
            float normalized_time = temps_ecoule / tempsRebond;//entre 0 et 1
            pos_y = maxHauteur + (minHauteur - maxHauteur) * normalized_time * normalized_time;
            if (pos_y > minHauteur) {
                pos_y = minHauteur;
            }
        } else {
            float cycles = temps_ecoule / tempsRebond;
            float amplitude = (minHauteur - maxHauteur) * pow(energiePerdue, floor(cycles));
            float phase = (cycles - floor(cycles)) * 2.0 * PI;
            pos_y = minHauteur - amplitude * (0.5 + 0.5 * sin(phase - PI/2.0));
        }

        //Dessin balle
        cercle(uint(pos_y), depart_x);
    }
}
