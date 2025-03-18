#define FOND 0x00000000
#define RAYON 50.0
#define PI 3.141592

//calc cercle euclidien
void cercle(uint cy, uint cx, int couleur) {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    float distance = sqrt(float((x - cx) * (x - cx) + (y - cy) * (y - cy)));
    
    if (distance <= RAYON) {
        data_0[p] = couleur;
    } else {
        data_0[p] = FOND;
    }
}

//cycle de couleurs
int couleur_rebond(int rebonds) {
    if (rebonds % 6 == 0) return 0xFFFFFFFF; // Blanc
    if (rebonds % 6 == 1) return 0xFF0000FF; // Rouge
    if (rebonds % 6 == 2) return 0xFF00FF00; // Vert
    if (rebonds % 6 == 3) return 0xFFFF0000; // Bleu
    if (rebonds % 6 == 4) return 0xFF00FFFF; // Jaune
    return 0xFFFF00FF; // Magenta
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

    //etat bouton souris
    bool mouse_pressed = mouse_button > 0;

    //stocke coordonnees de lancement
    bool depart_balle = (data_1[0] == 1);
    uint depart_x = uint(data_1[1]);
    uint depart_y = uint(data_1[2]);
    uint depart_step = uint(data_1[3]);

    //gestion lancement balle
    if (mouse_pressed && (!depart_balle || (mouse_x != depart_x || mouse_y != depart_y))) {
        //stocke animation
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
    
    
    //calc position
    if (depart_balle) {
        float temps_ecoule = float(step - depart_step);
        float maxHauteur = float(depart_y);
        float minHauteur = float(WSY - uint(RAYON));
        float tempsRebond = 90.0;
        float energiePerdue = 0.85;
                

        float pos_y;
        int nbRebonds = 0;

        //calc temps ecoule
        if (temps_ecoule < tempsRebond) {
            //premier rebond
            float normalized_time = temps_ecoule / tempsRebond;//entre 0 et 1
            pos_y = maxHauteur + (minHauteur - maxHauteur) * normalized_time * normalized_time;
            if (pos_y > minHauteur) {
                pos_y = minHauteur;
            }
        } else {
            //rebonds suivants
            float cycles = temps_ecoule / tempsRebond;
            nbRebonds = int(floor(cycles));
            float amplitude = (minHauteur - maxHauteur) * pow(energiePerdue, float(nbRebonds));
            float phase = (cycles - floor(cycles)) * 2.0 * PI;
            pos_y = minHauteur - amplitude * (0.5 + 0.5 * sin(phase - PI/2.0));
        }

        //couleur balle
        int couleur_actuelle = couleur_rebond(nbRebonds);

        //Dessin balle
        cercle(uint(pos_y) - WSY/6, depart_x + WSX/2, couleur_actuelle);
    }
}
