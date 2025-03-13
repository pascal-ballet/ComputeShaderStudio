#define RED 0xFFFF0000
#define GREEN 0xFF00FF00
#define BLUE 0xFF0000FF
#define BLACK 0xFF000000

// Generation de bullets
#define PI 3.1415926
#define TWO_PI 2*PI
#define SPAWN vec2(127,63)
#define RAD 5
#define NB_SPWN 12
#define OFFSET_ROTATION PI/20
#define SPEED 2.0

// Aleatoire pour le deplacement des bullets
#define PHI 1.61803398874989484820459
#define RND_MOVE 0.5

float compute_seed()
{
	float seed = 0.0;
	int mult = 10;
	int tmp = step;
	while(true)
	{
		if(tmp == 0) break;
		seed += (tmp%10)/mult;
		tmp = tmp/10;
		mult *= 10;
	}
	return seed;
}

float gold_noise(vec2 xy, float seed)
{
  return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

/* Layers :
 * data_0 : render
 * data_1 : position
 * data_2 : vector.x for movement (*1000)
 * data_3 : vector.y for movement (*1000)
 */

// Fonction d'initialisation des points d'apparition des bullets
// 
void init(uint p)
{
	data_1[p] = 0;
	data_2[p] = 0;
	data_3[p] = 0;
}

vec4 compute_spawn_position_angle(int nb, int r)
{
	vec4 position_direction;

	position_direction.x = SPAWN.x + cos(nb * (TWO_PI/NB_SPWN) + r*OFFSET_ROTATION) * RAD;
	position_direction.y = SPAWN.y + sin(nb * (TWO_PI/NB_SPWN) + r*OFFSET_ROTATION) * RAD;

	position_direction.z = (position_direction.x - SPAWN.x)*1000;
	position_direction.w = (position_direction.y - SPAWN.y)*1000;

	return position_direction;
}

void spawn_bullet(uint x, uint y, uint p, vec4 position_direction)
{
	if((int(position_direction.x) == int(x)) && (int(position_direction.y) == int(y))){
		data_1[p] = 1;
		data_2[p] = int(position_direction.z);
		data_3[p] = int(position_direction.w);
	}
}

// Fonction de calcul du deplacement de la bullet
void compute_movement(uint x, uint y, uint p)
{
	if(data_1[p] == 1){
		float seed = fract(compute_seed());
		if(gold_noise(vec2(int(x), int(y)), seed) <= RND_MOVE){
			// New position 
			// Current position + movement based on vec2
			vec2 dir = vec2(float(data_2[p])/1000, float(data_3[p])/1000);

			float angle = atan(dir.y, dir.x);
			float bx = float(x) + round(cos(angle)*SPEED);
			float by = float(y) + round(sin(angle)*SPEED);

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
}

void render(uint x, uint y, uint p)
{
	if(data_1[p] == 1 || data_1[p] == -1)
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
		init(p);
		
	}
	else{		
		spawn_bullet(x,y,p,compute_spawn_position_angle(step%NB_SPWN, 1));
		// Calcul du deplacement puis affichage du rendu
		if(step%2 == 0)
			compute_movement(x,y,p);
		render(x,y,p);
	}
}