// DEFINITIONS DES COULEURS ET ÉTATS
#define VIDE 0
#define JOUEUR 0xFF0000FF  // Rouge - Joueur humain
#define IA_1 0xFF00FF00    // Vert - IA 1
#define IA_2 0xFFFF0000    // Bleu - IA 2
#define IA_3 0xFFFFFF00    // Jaune - IA 3

// PARAMÈTRES DU JEU
#define FORCE_EXPANSION 0.15  // Probabilité d'expansion (0-1)
#define FORCE_CONQUETE 0.08   // Probabilité de conquête d'un territoire adverse (0-1)

// Fonction pseudo-aléatoire basée sur les coordonnées et le step
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

// Fonction pour déterminer si un événement aléatoire se produit
bool random_event(int x, int y, int step, float probability) {
    int random_value = hash(x, y, step);
    return (float(random_value) / 2147483647.0) < probability;
}

void main() {
    // Récupération des coordonnées
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);
    
    if (step == 0) {
        // INITIALISATION: PLACER LES TERRITOIRES DE DÉPART
        if (x < int(WSX)/6 && y < int(WSY)/6) {
            // Territoire du joueur (en haut à gauche)
            data_0[p] = JOUEUR;
        } else if (x > int(WSX)*5/6 && y < int(WSY)/6) {
            // IA 1 (en haut à droite)
            data_0[p] = IA_1;
        } else if (x < int(WSX)/6 && y > int(WSY)*5/6) {
            // IA 2 (en bas à gauche)
            data_0[p] = IA_2;
        } else if (x > int(WSX)*5/6 && y > int(WSY)*5/6) {
            // IA 3 (en bas à droite)
            data_0[p] = IA_3;
        } else {
            // Reste de la carte vide
            data_0[p] = VIDE;
        }
    } else {
        // LOGIQUE D'EXPANSION ET DE CONQUÊTE
        int couleur_actuelle = data_0[p];
        
        // SI LA CELLULE A UN PROPRIÉTAIRE
        if (couleur_actuelle != VIDE) {
            // TENTER D'ÉTENDRE LE TERRITOIRE
            if (random_event(x, y, step, FORCE_EXPANSION)) {
                // Choisir une direction aléatoire
                int direction = hash(x, y, step) % 8;
                int dx = 0, dy = 0;
                
                // 8 directions possibles
                switch (direction) {
                    case 0: dx = -1; dy = 0; break;  // Gauche
                    case 1: dx = 1; dy = 0; break;   // Droite
                    case 2: dx = 0; dy = -1; break;  // Haut
                    case 3: dx = 0; dy = 1; break;   // Bas
                    case 4: dx = -1; dy = -1; break; // Diagonale haut-gauche
                    case 5: dx = 1; dy = -1; break;  // Diagonale haut-droite
                    case 6: dx = -1; dy = 1; break;  // Diagonale bas-gauche
                    case 7: dx = 1; dy = 1; break;   // Diagonale bas-droite
                }
                
                // Calculer les nouvelles coordonnées
                int nx = x + dx;
                int ny = y + dy;
                
                // Vérifier les limites
                if (nx >= 0 && nx < int(WSX) && ny >= 0 && ny < int(WSY)) {
                    int voisin_p = nx + ny * int(WSX);
                    int couleur_voisin = data_0[voisin_p];
                    
                    // Si le voisin est vide, l'occuper
                    if (couleur_voisin == VIDE) {
                        data_0[voisin_p] = couleur_actuelle;
                    }
                    // Si le voisin appartient à un autre joueur, tenter de le conquérir
                    else if (couleur_voisin != couleur_actuelle && random_event(x, y, step, FORCE_CONQUETE)) {
                        data_0[voisin_p] = couleur_actuelle;
                    }
                }
            }
        } 
        // SI LA CELLULE EST VIDE
        else {
            // COMPTAGE DES VOISINS PAR COULEUR
            int nb_voisins_joueur = 0;
            int nb_voisins_ia1 = 0;
            int nb_voisins_ia2 = 0;
            int nb_voisins_ia3 = 0;
            
            // Parcours des 8 cellules voisines
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (!(dx == 0 && dy == 0)) {
                        int nx = x + dx;
                        int ny = y + dy;
                        
                        if (nx >= 0 && nx < int(WSX) && ny >= 0 && ny < int(WSY)) {
                            int voisin = data_0[nx + ny * int(WSX)];
                            if (voisin == JOUEUR) nb_voisins_joueur++;
                            else if (voisin == IA_1) nb_voisins_ia1++;
                            else if (voisin == IA_2) nb_voisins_ia2++;
                            else if (voisin == IA_3) nb_voisins_ia3++;
                        }
                    }
                }
            }
            
            // DÉTERMINER LE PROPRIÉTAIRE MAJORITAIRE PARMI LES VOISINS
            int max_voisins = max(nb_voisins_joueur, max(nb_voisins_ia1, max(nb_voisins_ia2, nb_voisins_ia3)));
            
            // S'il y a au moins 2 voisins de la même couleur, coloniser
            if (max_voisins >= 2) {
                if (max_voisins == nb_voisins_joueur) data_0[p] = JOUEUR;
                else if (max_voisins == nb_voisins_ia1) data_0[p] = IA_1;
                else if (max_voisins == nb_voisins_ia2) data_0[p] = IA_2;
                else if (max_voisins == nb_voisins_ia3) data_0[p] = IA_3;
            }
            
            // INTERACTION AVEC LA SOURIS (SIMULATION)
            // À chaque multiple de 100 steps, simuler un clic dans une zone aléatoire pour l'expansion du joueur
            if (step % 100 == 0) {
                // Calculer une position où le joueur "clique"
                int click_x = (hash(0, step, 0) % int(WSX));
                int click_y = (hash(step, 0, 0) % int(WSY));
                
                // Si nous sommes près de la position du "clic" et à proximité d'un territoire du joueur
                if (abs(x - click_x) < 5 && abs(y - click_y) < 5 && nb_voisins_joueur > 0) {
                    data_0[p] = JOUEUR;  // Conquête par le joueur
                }
            }
        }
    }
}