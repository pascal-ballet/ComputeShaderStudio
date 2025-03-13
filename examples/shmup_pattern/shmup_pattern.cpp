#define RED 0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE 0xFFFF0000
#define BLACK 0xFF000000

// Generation de bullets
#define PI 3.1415926
#define TWO_PI 2*PI
#define SPAWN1 vec2(63,31)
#define SPAWN2 vec2(191,31)
#define RAD1 50
#define RAD2 5
#define NB_SPWN1 10
#define NB_SPWN2 3
#define OFFSET_ROTATION PI/4
#define SPEED 2.0

// Aleatoire pour le deplacement des bullets
#define RND_MOVE 0.9

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

vec4 compute_spawn_position_angle(vec2 spawn, int nb, int nbRot, int rad, int nb_spwn)
{
	vec4 position_direction;

	position_direction.x = spawn.x + cos(nb * (TWO_PI/nb_spwn) + nbRot*OFFSET_ROTATION) * rad;
	position_direction.y = spawn.y + sin(nb * (TWO_PI/nb_spwn) + nbRot*OFFSET_ROTATION) * rad;

	position_direction.z = (position_direction.x - spawn.x)*1000;
	position_direction.w = (position_direction.y - spawn.y)*1000;

	return position_direction;
}

void spawn_bullet(uint x, uint y, uint p, vec4 position_direction, int bullet_type)
{
	if((int(position_direction.x) == int(x)) && (int(position_direction.y) == int(y))){
		data_1[p] = bullet_type;
		data_2[p] = int(position_direction.z);
		data_3[p] = int(position_direction.w);
	}
}


float compute_seed()
{
	int mult = 1;
	int tmp = 2*step+current_pass;
	bool loop = true;
	while(loop)
	{
		if(tmp == 0) loop = false;
		mult *= 10;
		tmp = tmp/10;
	}
	return step/mult;
}

float random(float seed)
{
  return fract(sin(seed)*2.85/cos(seed)*3.75);
}


// Fonction de calcul du deplacement de la bullet
void compute_movement(uint x, uint y, uint p, int bullet_type)
{
	if(data_1[p] == bullet_type){
		float random = random(float(int(x)*int(y)+2*step+current_pass));
		
		if(random <= RND_MOVE){
			// New position 
			// Current position + movement based on vec2
			vec2 dir = vec2(float(data_2[p])/1000, float(data_3[p])/1000);

			float angle = atan(dir.y, dir.x);
			float bx = float(x) + round(cos(angle)*SPEED);
			float by = float(y) + round(sin(angle)*SPEED);

			uint dp = uint(bx) + uint(by)*WSX;
			if((bx>0 && bx<WSX) && (by>0 && by<WSY)){
				if(data_1[dp] == 0){
					data_1[p] = 0;
					data_1[dp] = bullet_type;

					data_2[dp] = data_2[p];
					data_2[p] = 0;

					data_3[dp] = data_3[p];
					data_3[p] = 0;
				}
			}
			else{
				data_1[p] = 0;
				data_2[p] = 0;
				data_3[p] = 0;
			}
		}

	}
}

void compute_global_movement(uint x, uint y, uint p)
{
	compute_movement(x,y,p,1);
	compute_movement(x,y,p,2);
}

void render(uint x, uint y, uint p)
{
	if(data_1[p] == 1)
		data_0[p] = GREEN;
	if(data_1[p] == 2)
		data_0[p] = RED;
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
		// Apparition des bullets
		spawn_bullet(x,y,p,compute_spawn_position_angle(SPAWN1, step%NB_SPWN1, step, RAD1, NB_SPWN1), 1);
		spawn_bullet(x,y,p,compute_spawn_position_angle(SPAWN2, step%NB_SPWN2, step, RAD2, NB_SPWN2), 2);
		// Calcul du deplacement puis affichage du rendu
		if(step%2 == 0)
			compute_global_movement(x,y,p);
		render(x,y,p);
	}
}