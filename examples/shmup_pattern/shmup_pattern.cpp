#define RED 0xFF0000FF
#define PINK 0xFFFE25D5
#define GREEN 0xFF00FF00
#define BLUE 0xFFFF0000
#define CYAN 0xFFF8F901
#define BLACK 0xFF000000
#define GREY 0xFF2c2c2c

// Generation de bullets
#define PI 3.1415926
#define TWO_PI 2*PI
#define SPAWN1 vec2(63,31)
#define SPAWN2 vec2(191,63)
#define SPEED 2.0

// Aleatoire pour le deplacement des bullets
#define RND_MOVE 1.0

/* Layers :
 * data_0 : render
 * data_1 : position
 * data_2 : vector.x for movement (*1000)
 * data_3 : vector.y for movement (*1000)
 * data_4 : parameters for pattern management
 */


void init_pattern_parameters()
{
	// Pattern manager 
	data_4[0] = 0;
	data_4[1] = 4;
	data_4[2] = 0;

	// Initial parameter values for patterns
	// Pattern1
	data_4[0+WSX] = 0;
	data_4[1+WSX] = 0;
	data_4[2+WSX] = 0;
	data_4[3+WSX] = 0;
	// Pattern2
	data_4[4+WSX] = 0;
	data_4[5+WSX] = 0;
	data_4[6+WSX] = 0;
	data_4[7+WSX] = 0;

	// Parameters values
	// NB_SPAWN
	data_4[0+WSX*2] = 0;
	data_4[1+WSX*2] = 1;
	data_4[2+WSX*2] = 2;
	data_4[3+WSX*2] = 3;
	data_4[4+WSX*2] = 4;
	data_4[5+WSX*2] = 5;
	data_4[6+WSX*2] = 6;
	data_4[7+WSX*2] = 8;
	data_4[8+WSX*2] = 12;

	// RADIUS
	data_4[0+WSX*3] = 5;
	data_4[1+WSX*3] = 10;
	data_4[2+WSX*3] = 20;
	data_4[3+WSX*3] = 30;
	data_4[4+WSX*3] = 50;

	// INITIAL_OFFSET
	data_4[0+WSX*4] = 0;
	data_4[1+WSX*4] = 4;
	data_4[2+WSX*4] = 6;
	data_4[3+WSX*4] = 8;

	// DYNAMIC_OFFSET
	data_4[0+WSX*5] = 0;
	data_4[1+WSX*5] = 6;
	data_4[2+WSX*5] = 8;
	data_4[3+WSX*5] = 12;
	data_4[4+WSX*5] = 24;

	// SIZE for parameter choices
	data_4[0+WSX*6] = 9;
	data_4[1+WSX*6] = 5;
	data_4[2+WSX*6] = 4;
	data_4[3+WSX*6] = 5;

	// Button management
	data_4[0+WSX*7] = 0;
	data_4[1+WSX*7] = 0;
	data_4[2+WSX*7] = 0;
}


// Fonction d'initialisation des points d'apparition des bullets
// 
void init(uint p)
{
	data_1[p] = 0;
	data_2[p] = 0;
	data_3[p] = 0;

	if(p == 0){
		init_pattern_parameters();
	}
}

vec4 compute_spawn_position_angle(vec2 spawn, int pat)
{
	vec4 position_direction;
	float initial_offset;
 	float dynamic_offset;

 	int nb_spwn 					= data_4[data_4[(pat*4)+WSX]+WSX*2];
 	int rad 							= data_4[data_4[(pat*4)+1+WSX]+WSX*(3)];
  initial_offset 				= (data_4[data_4[(pat*4)+2+WSX]+WSX*(4)] == 0 ? 0 : PI/data_4[data_4[(pat*4)+2+WSX]+WSX*(4)]);
 	dynamic_offset			 	= (data_4[data_4[(pat*4)+3+WSX]+WSX*(5)] == 0 ? 0 : PI/data_4[data_4[(pat*4)+3+WSX]+WSX*(5)]);

	int nb = step%nb_spwn;

	position_direction.x = spawn.x + cos(nb * (TWO_PI/nb_spwn) + initial_offset + step*dynamic_offset) * rad;
	position_direction.y = spawn.y + sin(nb * (TWO_PI/nb_spwn) + initial_offset + step*dynamic_offset) * rad;

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
		data_0[p] = CYAN;
	if(data_1[p] == 2)
		data_0[p] = PINK;
}


void click_manager()
{
	if(mouse_button == 1)
		data_4[0+WSX*7] += 1;
	if(mouse_button == 2)
		data_4[1+WSX*7] += 1;
	if(mouse_button == 4)
		data_4[2+WSX*7] += 1;
}

void control_management()
{
	if(data_4[2+WSX*7] != 0){
		data_4[2+WSX*7] = 0;
		data_4[2] = (data_4[2]+1)%2;
	}

	if(data_4[1+WSX*7] != 0){
		data_4[1+WSX*7] = 0;
		int pat = data_4[2];
		data_4[pat] = (data_4[pat]+1)%4 + (pat*4);
	}
	
	if(data_4[0+WSX*7] != 0){
		data_4[0+WSX*7] = 0;
		int pat = data_4[2];
		int s = data_4[(data_4[pat]%4)+WSX*6];
		data_4[data_4[pat]+WSX] = (data_4[data_4[pat]+WSX]+1)%s;
	}
}


void main()
{
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint p = x + y * WSX;

	data_0[p] = GREY;
	
	if(step == 0){
		// Initialisation du point d'apparition des bullets
		init(p);
	}
	else{
		// Apparition des bullets
		spawn_bullet(x,y,p,compute_spawn_position_angle(SPAWN1, 0), 1);
		spawn_bullet(x,y,p,compute_spawn_position_angle(SPAWN2, 1), 2);

		// Check for interactions
		click_manager();
		if(mouse_button == 0)
			control_management();

		// Calcul du deplacement puis affichage du rendu
		if(step%2 == 0)
			compute_global_movement(x,y,p);
		render(x,y,p);
	}
}