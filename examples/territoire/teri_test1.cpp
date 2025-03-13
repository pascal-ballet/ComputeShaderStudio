#define MORT 0
#define VIVANT 0xFFFFFFFF
#define Rouge 0xFF0000FF  
#define Vert 0xFF00FF00    
#define Bleu 0xFFFF0000    
#define Jaune 0xFFFFFF00  


#define MIN_VOISINS_SURVIE 1
#define MAX_VOISINS_SURVIE 2
#define VOISINS_NAISSANCE 1


void compute_next_step(uint x, uint y, uint p){
	
	int nb_voisins_V = 0;
	int nb_voisins_R = 0;
	int nb_voisins_J = 0;
	int nb_voisins_B = 0;
	int nb_voisins_G = 0;

	// Comptage des voisins vivants
	for (int dy = -1; dy <= 1; dy++) {
		for (int dx = -1; dx <= 1; dx++) {
			if (!(dx == 0 && dy == 0)) {
				int nx = int(x) + dx;
				int ny = int(y) + dy;

				if (nx >= 0 && nx < WSX && ny >= 0 && ny < WSY) {
					if (data_0[uint(nx + ny * WSX)] == VIVANT) {
						nb_voisins_V++;
					}
					if (data_0[uint(nx + ny * WSX)] == Rouge) {
					 nb_voisins_R++;
					 }
					 if (data_0[uint(nx + ny * WSX)] == Vert) {
						 nb_voisins_G++;
					 }
					 if (data_0[uint(nx + ny * WSX)] == Bleu) {
						 nb_voisins_B++;
					 }
					 if (data_0[uint(nx + ny * WSX)] == Jaune) {
						 nb_voisins_J++;
					 }
				}
			}
		}
	}

	// La cellule morte devient vivante si elle a 1 voisin vivant
	if (data_0[p] == MORT) {
		if (nb_voisins_V >= 1) {
			data_1[p] = VIVANT;
		} else if(nb_voisins_R >= 1){
		 data_1[p] = Rouge;
		}else if(nb_voisins_J >= 1){
		 data_1[p] = Jaune;
		}else if(nb_voisins_G >= 1){
		 data_1[p] = Vert;
		}else if(nb_voisins_B >= 1){
		 data_1[p] = Bleu;
		}
	}
}


void main() {
   uint x = gl_GlobalInvocationID.x;
   uint y = gl_GlobalInvocationID.y;
   uint p = x + y * WSX;

   if (step == 0) {
       if (data_0[p] < 0) {
           if (x == WSX / 2 && y == WSY / 2) {  // On place une cellule vivante au centre
               data_0[p] = VIVANT;
           } else if (x == 1 && y== 1){
			data_0[p] = Rouge;
		   }else if(x == WSX-1 && y== 1){
			data_0[p] = Vert;
		   }else if(x == 1 && y== WSY-1){
			data_0[p] = Bleu;
		   }else if(x == WSX-1 && y== WSY-1){
			data_0[p] = Jaune;
		   } else  {
               data_0[p] = MORT;
           }
       } else {
           data_0[p] = MORT;
       }
   } else {
	if (current_pass == 0)
			compute_next_step(x, y, p);
		else
			data_0[p] = data_1[p]; // The future is now
   }
}
