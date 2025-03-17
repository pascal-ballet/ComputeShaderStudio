#define NEUTRE 0xFF000000
#define JOUEUR 0xFF0000FF  
#define IA_1 0xFF00FF00    
#define IA_2 0xFFFF0000    
#define IA_3 0xFFFFFF00    

// Couleurs pour afficher les scores
#define SCORE_BG_JOUEUR 0xFFDDDDFF
#define SCORE_BG_IA_1 0xFFDDFFDD
#define SCORE_BG_IA_2 0xFFFFDDDD
#define SCORE_BG_IA_3 0xFFFFFFDD
#define ZONE_DEMILITARISEE 0xFF888888

#define AI_EXPANSION_RATE 0.2

// Stockage des coordonnees de la souris
#define MOUSE_STORAGE_X 0
#define MOUSE_STORAGE_Y 1
#define MOUSE_STORAGE_CLICKED 2
#define MOUSE_BUTTON_PRESSED 3

// Stockage des scores
#define SCORE_JOUEUR 4
#define SCORE_IA_1 5
#define SCORE_IA_2 6
#define SCORE_IA_3 7

// Taille de la zone demilitarisee
#define ZONE_DEM_SIZE 40

// Verifier si un point est dans la zone demilitarisee 
bool is_demilitarized_zone(int x, int y) {
    int centerX = int(WSX) / 2;
    int centerY = int(WSY) / 2;
    
    return (x >= centerX - ZONE_DEM_SIZE / 2 && x < centerX + ZONE_DEM_SIZE / 2 && 
            y >= centerY - ZONE_DEM_SIZE / 2 && y < centerY + ZONE_DEM_SIZE / 2);
}

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

// Fonction pour calculer et mettre a jour les scores
void calculate_scores() {
    if (gl_GlobalInvocationID.x == 0 && gl_GlobalInvocationID.y == 0) {
        // Initialiser les scores a zero
        data_0[SCORE_JOUEUR] = 0;
        data_0[SCORE_IA_1] = 0;
        data_0[SCORE_IA_2] = 0;
        data_0[SCORE_IA_3] = 0;
        
        // Parcourir toute la grille pour compter les territoires
        for (int i = 0; i < int(WSX) * int(WSY); i++) {
            // Ne compter que les pixels qui ne sont pas dans la zone demilitarisee
            int px = i % int(WSX);
            int py = i / int(WSX);
            
            if (!is_demilitarized_zone(px, py)) {
                if (data_0[i] == JOUEUR) {
                    data_0[SCORE_JOUEUR]++;
                } else if (data_0[i] == IA_1) {
                    data_0[SCORE_IA_1]++;
                } else if (data_0[i] == IA_2) {
                    data_0[SCORE_IA_2]++;
                } else if (data_0[i] == IA_3) {
                    data_0[SCORE_IA_3]++;
                }
            }
        }
    }
}

// Affiche un chiffre a la position donnee
void draw_digit(int digit, int startX, int startY, int color) {
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);
    
    // Verification si nous sommes dans la region du chiffre
    if (x >= startX && x < startX + 3 && y >= startY && y < startY + 5) {
        int localX = x - startX;
        int localY = y - startY;
        bool pixel_on = false;
        
        // Representation minimaliste des chiffres en 3x5 pixels
        switch (digit) {
            case 0:
                pixel_on = (localX == 0 || localX == 2 || localY == 0 || localY == 4) || 
                           (localX == 1 && (localY == 0 || localY == 4));
                break;
            case 1:
                pixel_on = (localX == 1) || (localY == 4 && localX != 0);
                break;
            case 2:
                pixel_on = (localY == 0 || localY == 2 || localY == 4) || 
                           (localX == 2 && localY == 1) || (localX == 0 && localY == 3);
                break;
            case 3:
                pixel_on = (localY == 0 || localY == 2 || localY == 4) || (localX == 2);
                break;
            case 4:
                pixel_on = (localX == 0 && localY < 3) || (localX == 2) || (localY == 2);
                break;
            case 5:
                pixel_on = (localY == 0 || localY == 2 || localY == 4) || 
                           (localX == 0 && localY == 1) || (localX == 2 && localY == 3);
                break;
            case 6:
                pixel_on = (localY == 0 || localY == 2 || localY == 4) || 
                           (localX == 0) || (localX == 2 && localY > 2);
                break;
            case 7:
                pixel_on = (localY == 0) || (localX == 2);
                break;
            case 8:
                pixel_on = (localY == 0 || localY == 2 || localY == 4) || 
                           (localX == 0 || localX == 2);
                break;
            case 9:
                pixel_on = (localY == 0 || localY == 2 || localY == 4) || 
                           (localX == 2) || (localX == 0 && localY < 3);
                break;
        }
        
        if (pixel_on) {
            data_0[p] = color;
        }
    }
}

// Fonction pour afficher les scores dans la zone demilitarisee
void draw_scores_in_demilitarized_zone() {
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    
    if (is_demilitarized_zone(x, y)) {
        int centerX = int(WSX) / 2;
        int centerY = int(WSY) / 2;
        int p = x + y * int(WSX);
        
        // Fond de la zone demilitarisee
        data_0[p] = ZONE_DEMILITARISEE;
        
        // Recuperer les scores et les diviser par 10
        int score_joueur = data_0[SCORE_JOUEUR] / 10;
        int score_ia_1 = data_0[SCORE_IA_1] / 10;
        int score_ia_2 = data_0[SCORE_IA_2] / 10;
        int score_ia_3 = data_0[SCORE_IA_3] / 10;
        
        // Quadrant haut gauche - Joueur
        if (x < centerX && y < centerY) {
            data_0[p] = SCORE_BG_JOUEUR;
            
            // Afficher le score du joueur
            // Calculer les chiffres individuels (jusqu'a 3 chiffres)
            int hundreds = score_joueur / 100;
            int tens = (score_joueur / 10) % 10;
            int ones = score_joueur % 10;
            
            // Position de depart pour l'affichage des chiffres
            int startX = centerX - ZONE_DEM_SIZE / 2 + 3;
            int startY = centerY - ZONE_DEM_SIZE / 2 + 3;
            
            // Afficher chaque chiffre
            if (hundreds > 0) {
                draw_digit(hundreds, startX, startY, JOUEUR);
                draw_digit(tens, startX + 4, startY, JOUEUR);
                draw_digit(ones, startX + 8, startY, JOUEUR);
            } else if (tens > 0) {
                draw_digit(tens, startX + 2, startY, JOUEUR);
                draw_digit(ones, startX + 6, startY, JOUEUR);
            } else {
                draw_digit(ones, startX + 4, startY, JOUEUR);
            }
        }
        // Quadrant haut droite - IA_1
        else if (x >= centerX && y < centerY) {
            data_0[p] = SCORE_BG_IA_1;
            
            // Afficher le score de l'IA 1
            int hundreds = score_ia_1 / 100;
            int tens = (score_ia_1 / 10) % 10;
            int ones = score_ia_1 % 10;
            
            int startX = centerX + 3;
            int startY = centerY - ZONE_DEM_SIZE / 2 + 3;
            
            if (hundreds > 0) {
                draw_digit(hundreds, startX, startY, IA_1);
                draw_digit(tens, startX + 4, startY, IA_1);
                draw_digit(ones, startX + 8, startY, IA_1);
            } else if (tens > 0) {
                draw_digit(tens, startX + 2, startY, IA_1);
                draw_digit(ones, startX + 6, startY, IA_1);
            } else {
                draw_digit(ones, startX + 4, startY, IA_1);
            }
        }
        // Quadrant bas gauche - IA_2
        else if (x < centerX && y >= centerY) {
            data_0[p] = SCORE_BG_IA_2;
            
            // Afficher le score de l'IA 2
            int hundreds = score_ia_2 / 100;
            int tens = (score_ia_2 / 10) % 10;
            int ones = score_ia_2 % 10;
            
            int startX = centerX - ZONE_DEM_SIZE / 2 + 3;
            int startY = centerY + 3;
            
            if (hundreds > 0) {
                draw_digit(hundreds, startX, startY, IA_2);
                draw_digit(tens, startX + 4, startY, IA_2);
                draw_digit(ones, startX + 8, startY, IA_2);
            } else if (tens > 0) {
                draw_digit(tens, startX + 2, startY, IA_2);
                draw_digit(ones, startX + 6, startY, IA_2);
            } else {
                draw_digit(ones, startX + 4, startY, IA_2);
            }
        }
        // Quadrant bas droite - IA_3
        else if (x >= centerX && y >= centerY) {
            data_0[p] = SCORE_BG_IA_3;
            
            // Afficher le score de l'IA 3
            int hundreds = score_ia_3 / 100;
            int tens = (score_ia_3 / 10) % 10;
            int ones = score_ia_3 % 10;
            
            int startX = centerX + 3;
            int startY = centerY + 3;
            
            if (hundreds > 0) {
                draw_digit(hundreds, startX, startY, IA_3);
                draw_digit(tens, startX + 4, startY, IA_3);
                draw_digit(ones, startX + 8, startY, IA_3);
            } else if (tens > 0) {
                draw_digit(tens, startX + 2, startY, IA_3);
                draw_digit(ones, startX + 6, startY, IA_3);
            } else {
                draw_digit(ones, startX + 4, startY, IA_3);
            }
        }
    }
}

void main() {
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);
    
    // Initialisation
    if (step == 0) {
        // Verifier si le pixel est dans la zone demilitarisee
        if (is_demilitarized_zone(x, y)) {
            draw_scores_in_demilitarized_zone();
        } else {
            // Initialisation normale pour les autres pixels
            if (x < int(WSX)/8 && y < int(WSY)/8) {
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
        
        // Initialiser les scores
        if (x == 0 && y == 0) {
            data_0[SCORE_JOUEUR] = 0;
            data_0[SCORE_IA_1] = 0;
            data_0[SCORE_IA_2] = 0;
            data_0[SCORE_IA_3] = 0;
        }
    } 
    // Logique de jeu
    else {
        // Si on est dans la zone demilitarisee, on affiche les scores
        if (is_demilitarized_zone(x, y)) {
            draw_scores_in_demilitarized_zone();
        } else {
            // Sinon, logique de jeu normale
            
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
                    
                    // Verifier que le pixel voisin n'est pas dans la zone demilitarisee
                    if (nx >= 0 && nx < int(WSX) && ny >= 0 && ny < int(WSY) && !is_demilitarized_zone(nx, ny)) {
                        int voisin_p = nx + ny * int(WSX);
                        
                        if (data_0[voisin_p] == NEUTRE || (data_0[voisin_p] != JOUEUR && random_event(x, y, step, 3))) {
                            data_0[voisin_p] = JOUEUR;
                        }
                    }
                }
            }
            
            // IA expansion automatique - garde ce comportement pour equilibrer le jeu
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
                    
                    // Verifier que le pixel voisin n'est pas dans la zone demilitarisee
                    if (nx >= 0 && nx < int(WSX) && ny >= 0 && ny < int(WSY) && !is_demilitarized_zone(nx, ny)) {
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
                    
                    // Verifier que le pixel voisin n'est pas dans la zone demilitarisee
                    if (nx >= 0 && nx < int(WSX) && ny >= 0 && ny < int(WSY) && !is_demilitarized_zone(nx, ny)) {
                        int voisin_p = nx + ny * int(WSX);
                        
                        if (data_0[voisin_p] == NEUTRE || (data_0[voisin_p] != IA_2 && random_event(x, y, step, 0.2))) {
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
                    
                    // Verifier que le pixel voisin n'est pas dans la zone demilitarisee
                    if (nx >= 0 && nx < int(WSX) && ny >= 0 && ny < int(WSY) && !is_demilitarized_zone(nx, ny)) {
                        int voisin_p = nx + ny * int(WSX);
                        
                        if (data_0[voisin_p] == NEUTRE || (data_0[voisin_p] != IA_3 && random_event(x, y, step, 0.2))) {
                            data_0[voisin_p] = IA_3;
                        }
                    }
                }
            }
        }
        
        // Calculer les scores tous les steps
        if (step % 10 == 0 && x == 0 && y == 0) {
            calculate_scores();
        }
    }
}