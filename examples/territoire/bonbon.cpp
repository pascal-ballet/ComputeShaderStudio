#define NEUTRE 0xFF000000
#define JOUEUR 0xFF0000FF  
#define IA_1 0xFF00FF00    
#define IA_2 0xFFFF0000    
#define IA_3 0xFFFFFF00    
#define BLANC 0xFFFFFFFF
#define NOIR 0xFF000000

#define CLICK_RADIUS 25        // Rayon d'expansion augmenté
#define EXPANSION_AMOUNT 30    // Quantité d'expansion lors d'un clic
#define AI_EXPANSION_RATE 0.02

// Stockage des coordonnees de la souris
#define MOUSE_STORAGE_X 0
#define MOUSE_STORAGE_Y 1
#define MOUSE_STORAGE_CLICKED 2
#define MOUSE_BUTTON_PRESSED 3

int hash(int x, int y, int s) {
    int a = x * 0x6A09E667;
    int b = y * 0xBB67AE85;
    int c = s * 0x3C6EF372;
    int h = a + b + c;
    h = (h ^ (h >> 16)) * 0x85EBCA6B;
    h = (h ^ (h >> 13)) * 0xC2B2AE35;
    h = h ^ (h >> 16);
    return h;
}

bool random_event(int x, int y, int step, float probability) {
    int random_value = hash(x, y, step);
    float normalized = float(random_value) / 2147483647.0;
    return (normalized > 0.0 ? normalized : -normalized) < probability;
}

// Dessine un segment horizontal
void drawHorizontalLine(int start_x, int y, int length, int color) {
    for(int i = 0; i < length; i++) {
        int pos = (start_x + i) + y * int(WSX);
        if(start_x + i >= 0 && start_x + i < int(WSX) && y >= 0 && y < int(WSY)) {
            data_0[pos] = color;
        }
    }
}

// Dessine un segment vertical
void drawVerticalLine(int x, int start_y, int length, int color) {
    for(int i = 0; i < length; i++) {
        int pos = x + (start_y + i) * int(WSX);
        if(x >= 0 && x < int(WSX) && start_y + i >= 0 && start_y + i < int(WSY)) {
            data_0[pos] = color;
        }
    }
}

void main() {
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);
    
    // Initialisation
    if (step == 0) {
        // Premiere rangee pour stocker les infos de la souris
        if (y == 0 && x < 10) {
            data_0[p] = -1; // Initialiser a -1
        }
        // Territoire du joueur en haut a gauche
        else if (x < int(WSX)/8 && y < int(WSY)/8) {
            data_0[p] = JOUEUR;
        } 
        // IA 1 en haut a droite
        else if (x > int(WSX)*7/8 && y < int(WSY)/8) {
            data_0[p] = IA_1;
        } 
        // IA 2 en bas a gauche
        else if (x < int(WSX)/8 && y > int(WSY)*7/8) {
            data_0[p] = IA_2;
        } 
        // IA 3 en bas a droite
        else if (x > int(WSX)*7/8 && y > int(WSY)*7/8) {
            data_0[p] = IA_3;
        } 
        // Reste de la carte neutre
        else {
            data_0[p] = NEUTRE;
        }
    } 
    // Logique de jeu
    else {
        // Gestion du clic de souris
        bool clicked = false;
        
        // Verifier si c est un nouveau clic seulement au debut de la frame
        if (y == 0 && x == 0) {
            int lastX = data_0[MOUSE_STORAGE_X];
            int lastY = data_0[MOUSE_STORAGE_Y];
            bool lastButtonState = data_0[MOUSE_BUTTON_PRESSED] > 0;
            bool currentButtonState = mouse_button > 0;
            
            // Detecter un clic seulement quand le bouton vient d'etre presse
            if (currentButtonState && !lastButtonState) {
                clicked = true;
                data_0[MOUSE_STORAGE_CLICKED] = 1;
            } else {
                data_0[MOUSE_STORAGE_CLICKED] = 0;
            }
            
            // Sauvegarder la position et l'etat du bouton
            data_0[MOUSE_STORAGE_X] = mousex;
            data_0[MOUSE_STORAGE_Y] = mousey;
            data_0[MOUSE_BUTTON_PRESSED] = currentButtonState ? 1 : 0;
        }
        
        clicked = data_0[MOUSE_STORAGE_CLICKED] == 1;
        
        // Interaction joueur - expansion par clic
        if (clicked && mousex >= 0 && mousey >= 0) {

            if (data_0[p] == JOUEUR && random_event(x, y, step, AI_EXPANSION_RATE * 10000000)) {
                int direction = hash(x, y, step) % 8;
                int dx = 0, dy = 0;
                
                switch (direction) {
                    case 0: dx = -1; dy = 0; break;
                    case 1: dx = 1; dy = 0; break;
                    case 2: dx = 0; dy = -1; break;
                    case 3: dx = 0; dy = 1; break;
                    case 4: dx = -1; dy = -1; break;
                    case 5: dx = 1; dy = -1; break;
                    case 6: dx = -1; dy = 1; break;
                    case 7: dx = 1; dy = 1; break;
                }
                
                int nx = x + dx;
                int ny = y + dy;
                
                if (nx >= 0 && nx < int(WSX) && ny >= 0 && ny < int(WSY)) {
                    int voisin_p = nx + ny * int(WSX);
                    
                    if (data_0[voisin_p] == NEUTRE || (data_0[voisin_p] != JOUEUR && random_event(x, y, step, 3))) {
                        data_0[voisin_p] = JOUEUR;
                    }
                }
            }
        }
        
        // IA expansion automatique - garde ce comportement pour équilibrer le jeu
        if (step % 3 == 0) { // Ralentir IA pour equilibrer
            // IA 1 Plus agressive
            if (data_0[p] == IA_1 && random_event(x, y, step, AI_EXPANSION_RATE * 1.5)) {
                int direction = hash(x, y, step) % 8;
                int dx = 0, dy = 0;
                
                switch (direction) {
                    case 0: dx = -1; dy = 0; break;
                    case 1: dx = 1; dy = 0; break;
                    case 2: dx = 0; dy = -1; break;
                    case 3: dx = 0; dy = 1; break;
                    case 4: dx = -1; dy = -1; break;
                    case 5: dx = 1; dy = -1; break;
                    case 6: dx = -1; dy = 1; break;
                    case 7: dx = 1; dy = 1; break;
                }
                
                int nx = x + dx;
                int ny = y + dy;
                
                if (nx >= 0 && nx < int(WSX) && ny >= 0 && ny < int(WSY)) {
                    int voisin_p = nx + ny * int(WSX);
                    
                    if (data_0[voisin_p] == NEUTRE || (data_0[voisin_p] != IA_1 && random_event(x, y, step, 0.2))) {
                        data_0[voisin_p] = IA_1;
                    }
                }
            }
            
            // IA 2 Expansion normale
            else if (data_0[p] == IA_2 && random_event(x, y, step, AI_EXPANSION_RATE)) {
                int direction = hash(x, y, step) % 8;
                int dx = 0, dy = 0;
                
                switch (direction) {
                    case 0: dx = -1; dy = 0; break;
                    case 1: dx = 1; dy = 0; break;
                    case 2: dx = 0; dy = -1; break;
                    case 3: dx = 0; dy = 1; break;
                    case 4: dx = -1; dy = -1; break;
                    case 5: dx = 1; dy = -1; break;
                    case 6: dx = -1; dy = 1; break;
                    case 7: dx = 1; dy = 1; break;
                }
                
                int nx = x + dx;
                int ny = y + dy;
                
                if (nx >= 0 && nx < int(WSX) && ny >= 0 && ny < int(WSY)) {
                    int voisin_p = nx + ny * int(WSX);
                    
                    if (data_0[voisin_p] == NEUTRE) {
                        data_0[voisin_p] = IA_2;
                    }
                }
            }
            
            // IA 3 Plus defensive
            else if (data_0[p] == IA_3 && random_event(x, y, step, AI_EXPANSION_RATE * 0.8)) {
                int direction = hash(x, y, step) % 8;
                int dx = 0, dy = 0;
                
                switch (direction) {
                    case 0: dx = -1; dy = 0; break;
                    case 1: dx = 1; dy = 0; break;
                    case 2: dx = 0; dy = -1; break;
                    case 3: dx = 0; dy = 1; break;
                    case 4: dx = -1; dy = -1; break;
                    case 5: dx = 1; dy = -1; break;
                    case 6: dx = -1; dy = 1; break;
                    case 7: dx = 1; dy = 1; break;
                }
                
                int nx = x + dx;
                int ny = y + dy;
                
                if (nx >= 0 && nx < int(WSX) && ny >= 0 && ny < int(WSY)) {
                    int voisin_p = nx + ny * int(WSX);
                    
                    if (data_0[voisin_p] == NEUTRE) {
                        data_0[voisin_p] = IA_3;
                    }
                }
            }
        }
    }
}