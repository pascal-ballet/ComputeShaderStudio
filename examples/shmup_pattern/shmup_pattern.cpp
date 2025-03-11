#define RED 0xFFFF0000
#define GREEN 0xFF00FF00
#define BLUE 0xFF0000FF
#define BLACK 0xFF000000

#define PI 3.1415926
#define TWO_PI 2*PI
#define SPAWN vec2(32,32)
#define RAD 3
#define NB_SPWN 6

/* Layers :
 * data_0 : render
 * data_1 : position
 * data_2 : vector.x for movement (*1000)
 * data_3 : vector.y for movement (*1000)
 */

// Fonction d'initialisation des points d'apparition des bullets
// 
void init(uint x, uint y, uint p)
{
	float angle = TWO_PI/NB_SPWN;

	if(int(x)==SPAWN.x && int(y)==SPAWN.y){
		data_1[p] = 1;
		data_2[p] = 1000;
		data_3[p] = 0;
	}
	else{
		data_1[p] = 0;
		data_2[p] = 0;
		data_3[p] = 0;
	}
}

// Fonction de calcul du deplacement de la bullet
void compute_movement(uint x, uint y, uint p)
{
	if(data_1[p] == 1){
		// New position 
		// Current position + movement based on vec2
		vec2 dir = vec2(float(data_2[p])/1000, float(data_3[p])/1000);
		float speed = 1.0;
		float angle = atan(dir.y, dir.x);
		float bx = float(x) + round(cos(angle))*speed;
		float by = float(y) + round(sin(angle))*speed;

		uint dp = uint(bx) + uint(by)*WSX;
		if((bx>0 && bx<WSX) && (by>0 && by<WSY)){
			data_1[p] = 0;
			data_1[dp] = 1;

			data_2[dp] = data_2[p];
			data_2[p] = 0;

			data_3[dp] = data_3[p];
			data_3[p] = 0;
		}
		else{
			data_1[p] = 0;
			data_2[p] = 0;
			data_3[p] = 0;
		}

	}
}


void render(uint x, uint y, uint p)
{
	if(data_1[p] == 1)
		data_0[p] = GREEN;
}


void main()
{
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint p = x + y * WSX;

	data_0[p] = BLACK;
	
	if(step == 0){
		// Initialisation du point d'apparition des bullets
		init(x,y,p);
	}
	else{
		// Calcul du deplacement puis affichage du rendu
		if(step%2 == 0)
			compute_movement(x,y,p);
		render(x,y,p);
	}
}