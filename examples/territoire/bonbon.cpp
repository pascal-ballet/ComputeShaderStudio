#define NEUTRE 0xFF808080
#define JOUEUR 0xFF0000FF  
#define IA_1 0xFF00FF00    
#define IA_2 0xFFFF0000    
#define IA_3 0xFFFFFF00    
#define BLANC 0xFFFFFFFF
#define NOIR 0xFF000000

#define CLICK_RADIUS 15
#define AI_EXPANSION_RATE 0.03

// Stockage des coordonnees de la souris
#define MOUSE_STORAGE_X 0
#define MOUSE_STORAGE_Y 1
#define MOUSE_STORAGE_CLICKED 2

// Configuration de affichage du score
#define SCORE_Y int(WSY)/2
#define DIGIT_WIDTH 8 
#define DIGIT_HEIGHT 12
#define DIGIT_SPACING 10

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

// Dessine un chiffre
void drawDigit(int digit, int x, int y, int color) {
    switch(digit) {
        case 0:
            drawHorizontalLine(x, y, DIGIT_WIDTH, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT - 1, DIGIT_WIDTH, color);
            drawVerticalLine(x, y, DIGIT_HEIGHT, color);
            drawVerticalLine(x + DIGIT_WIDTH - 1, y, DIGIT_HEIGHT, color);
            break;
        case 1:
            drawVerticalLine(x + DIGIT_WIDTH - 2, y, DIGIT_HEIGHT, color);
            break;
        case 2:
            drawHorizontalLine(x, y, DIGIT_WIDTH, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT/2, DIGIT_WIDTH, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT - 1, DIGIT_WIDTH, color);
            drawVerticalLine(x + DIGIT_WIDTH - 1, y, DIGIT_HEIGHT/2, color);
            drawVerticalLine(x, y + DIGIT_HEIGHT/2, DIGIT_HEIGHT/2, color);
            break;
        case 3:
            drawHorizontalLine(x, y, DIGIT_WIDTH, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT/2, DIGIT_WIDTH, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT - 1, DIGIT_WIDTH, color);
            drawVerticalLine(x + DIGIT_WIDTH - 1, y, DIGIT_HEIGHT, color);
            break;
        case 4:
            drawVerticalLine(x, y, DIGIT_HEIGHT/2, color);
            drawVerticalLine(x + DIGIT_WIDTH - 1, y, DIGIT_HEIGHT, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT/2, DIGIT_WIDTH, color);
            break;
        case 5:
            drawHorizontalLine(x, y, DIGIT_WIDTH, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT/2, DIGIT_WIDTH, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT - 1, DIGIT_WIDTH, color);
            drawVerticalLine(x, y, DIGIT_HEIGHT/2, color);
            drawVerticalLine(x + DIGIT_WIDTH - 1, y + DIGIT_HEIGHT/2, DIGIT_HEIGHT/2, color);
            break;
        case 6:
            drawHorizontalLine(x, y, DIGIT_WIDTH, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT/2, DIGIT_WIDTH, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT - 1, DIGIT_WIDTH, color);
            drawVerticalLine(x, y, DIGIT_HEIGHT, color);
            drawVerticalLine(x + DIGIT_WIDTH - 1, y + DIGIT_HEIGHT/2, DIGIT_HEIGHT/2, color);
            break;
        case 7:
            drawHorizontalLine(x, y, DIGIT_WIDTH, color);
            drawVerticalLine(x + DIGIT_WIDTH - 1, y, DIGIT_HEIGHT, color);
            break;
        case 8:
            drawHorizontalLine(x, y, DIGIT_WIDTH, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT/2, DIGIT_WIDTH, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT - 1, DIGIT_WIDTH, color);
            drawVerticalLine(x, y, DIGIT_HEIGHT, color);
            drawVerticalLine(x + DIGIT_WIDTH - 1, y, DIGIT_HEIGHT, color);
            break;
        case 9:
            drawHorizontalLine(x, y, DIGIT_WIDTH, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT/2, DIGIT_WIDTH, color);
            drawHorizontalLine(x, y + DIGIT_HEIGHT - 1, DIGIT_WIDTH, color);
            drawVerticalLine(x, y, DIGIT_HEIGHT/2, color);
            drawVerticalLine(x + DIGIT_WIDTH - 1, y, DIGIT_HEIGHT, color);
            break;
    }
}

// Dessine un nombre
void drawNumber(int num, int x, int y, int color) {
    if(num == 0) {
        drawDigit(0, x, y, color);
        return;
    }
    
    int temp = num;
    int digits = 0;
    
    // Compter les chiffres
    while(temp > 0) {
        temp /= 10;
        digits++;
    }
    
    // Dessiner chaque chiffre
    temp = num;
    for(int i = 0; i < digits; i++) {
        int digit = temp % 10;
        drawDigit(digit, x + (digits-i-1) * DIGIT_SPACING, y, color);
        temp /= 10;
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
        if (y == 0 && x == 0 && mousex >= 0 && mousey >= 0) {
            int lastX = data_0[MOUSE_STORAGE_X];
            int lastY = data_0[MOUSE_STORAGE_Y];
            
            // Si la position a change c est un nouveau clic
            if (lastX != mousex || lastY != mousey) {
                clicked = true;
                data_0[MOUSE_STORAGE_X] = mousex;
                data_0[MOUSE_STORAGE_Y] = mousey;
                data_0[MOUSE_STORAGE_CLICKED] = 1;
            } else {
                data_0[MOUSE_STORAGE_CLICKED] = 0;
            }
        }
        
        clicked = data_0[MOUSE_STORAGE_CLICKED] == 1;
        
        // Interaction joueur expansion par clic
        if (clicked && mousex >= 0 && mousey >= 0) {
            // Distance au clic
            float dist = sqrt(float((x - mousex) * (x - mousex) + (y - mousey) * (y - mousey)));
            
            // Zone expansion autour du clic
            if (dist < CLICK_RADIUS) {
                // Verifier si le clic est proche d un territoire du joueur
                bool proche_joueur = false;
                
                for (int j = -3; j <= 3; j++) {
                    for (int i = -3; i <= 3; i++) {
                        int nx = mousex + i;
                        int ny = mousey + j;
                        
                        if (nx >= 0 && nx < int(WSX) && ny >= 0 && ny < int(WSY)) {
                            int check_p = nx + ny * int(WSX);
                            if (data_0[check_p] == JOUEUR) {
                                proche_joueur = true;
                                break;
                            }
                        }
                    }
                    if (proche_joueur) break;
                }
                
                // Si on est pres d un territoire du joueur on peut conquerir
                if (proche_joueur) {
                    // Probabilite de conquete qui diminue avec la distance
                    float proba = 1.0 - (dist / CLICK_RADIUS);
                    
                    if (random_event(x, y, step, proba)) {
                        // Conquete facile de territoire neutre
                        if (data_0[p] == NEUTRE) {
                            data_0[p] = JOUEUR;
                        }
                        // Conquete plus difficile de territoire ennemi
                        else if (data_0[p] != JOUEUR && random_event(x, y, step, proba * 0.5)) {
                            data_0[p] = JOUEUR;
                        }
                    }
                }
            }
        }
        
        // IA expansion automatique
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
        
        // Calcul des scores une seule fois par frame
        int score_joueur = 0;
        int score_ia1 = 0;
        int score_ia2 = 0;
        int score_ia3 = 0;
        
        if (x == 0 && y == 1) {
            for (int j = 0; j < int(WSY); j++) {
                for (int i = 0; i < int(WSX); i++) {
                    int idx = i + j * int(WSX);
                    if (data_0[idx] == JOUEUR) score_joueur++;
                    else if (data_0[idx] == IA_1) score_ia1++;
                    else if (data_0[idx] == IA_2) score_ia2++;
                    else if (data_0[idx] == IA_3) score_ia3++;
                }
            }
            
            // Stocker les scores dans les pixels speciaux
            data_0[int(WSX) - 4] = score_joueur;
            data_0[int(WSX) - 3] = score_ia1;
            data_0[int(WSX) - 2] = score_ia2;
            data_0[int(WSX) - 1] = score_ia3;
        }
        
        // Affichage des scores au milieu de ecran
        if (y >= SCORE_Y - DIGIT_HEIGHT && y < SCORE_Y + DIGIT_HEIGHT*2) {
            // Recuperer les scores
            score_joueur = data_0[int(WSX) - 4];
            score_ia1 = data_0[int(WSX) - 3];
            score_ia2 = data_0[int(WSX) - 2];
            score_ia3 = data_0[int(WSX) - 1];
            
            // Fond noir pour le score
            if (y == SCORE_Y - 5) {
                if (x > int(WSX)/2 - 50 && x < int(WSX)/2 + 150) {
                    data_0[p] = NOIR;
                }
            }
            
            // Afficher les scores
            if (x == int(WSX)/2 - 45 && y == SCORE_Y) {
                drawNumber(score_joueur, int(WSX)/2 - 45, SCORE_Y, JOUEUR);
                drawNumber(score_ia1, int(WSX)/2 + 15, SCORE_Y, IA_1);
                drawNumber(score_ia2, int(WSX)/2 - 45, SCORE_Y + DIGIT_HEIGHT + 5, IA_2);
                drawNumber(score_ia3, int(WSX)/2 + 15, SCORE_Y + DIGIT_HEIGHT + 5, IA_3);
            }
        }
        
        // Highlight du curseur
        if (x == mousex && y == mousey) {
            data_0[p] = BLANC;
        }
    }
}