#define MORT 0
#define VIVANT 0xFFFFFFFF

#define MIN_VOISINS_SURVIE 1
#define MAX_VOISINS_SURVIE 2
#define VOISINS_NAISSANCE 1


uint hash(uint x, uint y) {
    uint a = x * 0x6A09E667;
    uint b = y * 0xBB67AE85;
    uint h = a + b;
    h = (h ^ (h >> 16)) * 0x85EBCA6B;
    h = (h ^ (h >> 13)) * 0xC2B2AE35;
    h = h ^ (h >> 16);
    return h;
}

void main() {
   uint x = gl_GlobalInvocationID.x;
   uint y = gl_GlobalInvocationID.y;
   uint p = x + y * WSX;

   if (step == 0) {
       if (data_0[p] < 0) {
           if (x == WSX / 2 && y == WSY / 2) {  // On place une cellule vivante au centre
               data_0[p] = VIVANT;
           } else {
               data_0[p] = MORT;
           }
       } else {
           data_0[p] = MORT;
       }
   } else {
       int nb_voisins = 0;

       // Comptage des voisins vivants
       for (int dy = -1; dy <= 1; dy++) {
           for (int dx = -1; dx <= 1; dx++) {
               if (!(dx == 0 && dy == 0)) {
                   int nx = int(x) + dx;
                   int ny = int(y) + dy;

                   if (nx >= 0 && nx < WSX && ny >= 0 && ny < WSY) {
                       if (data_0[uint(nx + ny * WSX)] == VIVANT) {
                           nb_voisins++;
                       }
                   }
               }
           }
       }

       // La cellule morte devient vivante si elle a 1 voisin vivant
       if (data_0[p] == MORT) {
           if (nb_voisins >= VOISINS_NAISSANCE) {
               data_0[p] = VIVANT;
           }
       }
   }
}
