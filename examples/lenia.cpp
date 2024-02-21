// Write your code HERE
#define INTERVALL 1073741824
// old intervall 2147483647

//couleur de l'affichage
#define DEAD 0
#define DEAD_COLOR vec4(0.0f, 0.0f, 0.0f, 1.0f)  // Noir
#define ALIVE_COLOR vec4(0.8f, 0.8f, 0.0f, 1.0f) // Jaune
#define MID_ALIVE_COLOR vec4(1.0f, 0.5f, 0.0f, 1.0f) // orange
#define BELLOW_MID_ALIVE_COLOR vec4(1.0f, 0.0f, 1.0f, 1.0f) // violet


//rayon de calcule de chaque pixel
#define R 13

#define INIT 2
//init 0 random
//init 1 cercle
//init 2 orbium

//un animal
const float orbium[20][20] = float[20][20](
    float[20](0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.14f, 0.1f, 0.0f, 0.0f, 0.03f, 0.03f, 0.0f, 0.0f, 0.3f, 0.0f, 0.0f, 0.0f, 0.0f),
    float[20](0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.08f, 0.24f, 0.3f, 0.3f, 0.18f, 0.14f, 0.15f, 0.16f, 0.15f, 0.09f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f),
    float[20](0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.15f, 0.34f, 0.44f, 0.46f, 0.38f, 0.18f, 0.14f, 0.11f, 0.13f, 0.19f, 0.18f, 0.45f, 0.0f, 0.0f, 0.0f),
    float[20](0.0f, 0.0f, 0.0f, 0.0f, 0.06f, 0.13f, 0.39f, 0.5f, 0.5f, 0.37f, 0.06f, 0.0f, 0.0f, 0.0f, 0.02f, 0.16f, 0.68f, 0.0f, 0.0f, 0.0f),
    float[20](0.0f, 0.0f, 0.0f, 0.11f, 0.17f, 0.17f, 0.33f, 0.4f, 0.38f, 0.28f, 0.14f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.18f, 0.42f, 0.0f, 0.0f),
    float[20](0.0f, 0.0f, 0.09f, 0.18f, 0.13f, 0.06f, 0.08f, 0.26f, 0.32f, 0.32f, 0.27f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.82f, 0.0f, 0.0f),
    float[20](0.27f, 0.0f, 0.16f, 0.12f, 0.0f, 0.0f, 0.0f, 0.25f, 0.38f, 0.44f, 0.45f, 0.34f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.22f, 0.17f, 0.0f),
    float[20](0.0f, 0.07f, 0.2f, 0.02f, 0.0f, 0.0f, 0.0f, 0.31f, 0.48f, 0.57f, 0.6f, 0.57f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.49f, 0.0f),
    float[20](0.0f, 0.59f, 0.19f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.57f, 0.69f, 0.76f, 0.76f, 0.49f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.36f, 0.0f),
    float[20](0.0f, 0.58f, 0.19f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.67f, 0.83f, 0.9f, 0.92f, 0.87f, 0.12f, 0.0f, 0.0f, 0.0f, 0.0f, 0.22f, 0.07f),
    float[20](0.0f, 0.0f, 0.46f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.7f, 0.93f, 1.0f, 1.0f, 1.0f, 0.61f, 0.0f, 0.0f, 0.0f, 0.0f, 0.18f, 0.11f),
    float[20](0.0f, 0.0f, 0.82f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.47f, 1.0f, 1.0f, 0.98f, 1.0f, 0.96f, 0.27f, 0.0f, 0.0f, 0.0f, 0.19f, 0.1f),
    float[20](0.0f, 0.0f, 0.46f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.25f, 1.0f, 1.0f, 0.84f, 0.92f, 0.97f, 0.54f, 0.14f, 0.04f, 0.1f, 0.21f, 0.05f),
    float[20](0.0f, 0.0f, 0.0f, 0.4f, 0.0f, 0.0f, 0.0f, 0.0f, 0.09f, 0.8f, 1.0f, 0.82f, 0.8f, 0.85f, 0.63f, 0.31f, 0.18f, 0.19f, 0.2f, 0.01f),
    float[20](0.0f, 0.0f, 0.0f, 0.36f, 0.1f, 0.0f, 0.0f, 0.0f, 0.05f, 0.54f, 0.86f, 0.79f, 0.74f, 0.72f, 0.6f, 0.39f, 0.28f, 0.24f, 0.13f, 0.0f),
    float[20](0.0f, 0.0f, 0.0f, 0.01f, 0.3f, 0.07f, 0.0f, 0.0f, 0.08f, 0.36f, 0.64f, 0.7f, 0.64f, 0.6f, 0.51f, 0.39f, 0.29f, 0.19f, 0.04f, 0.0f),
    float[20](0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.24f, 0.14f, 0.1f, 0.15f, 0.29f, 0.45f, 0.53f, 0.52f, 0.46f, 0.4f, 0.31f, 0.21f, 0.08f, 0.0f, 0.0f),
    float[20](0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.08f, 0.21f, 0.21f, 0.22f, 0.29f, 0.36f, 0.39f, 0.37f, 0.33f, 0.26f, 0.18f, 0.09f, 0.0f, 0.0f, 0.0f),
    float[20](0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.03f, 0.13f, 0.19f, 0.22f, 0.24f, 0.24f, 0.23f, 0.18f, 0.13f, 0.05f, 0.0f, 0.0f, 0.0f, 0.0f),
    float[20](0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.02f, 0.06f, 0.08f, 0.09f, 0.07f, 0.05f, 0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)
);

//fonction random
float random (vec2 uv) {
    return fract(sin(dot(uv.xy,
        vec2(12.9898f,78.233f))) * 43758.5453123f);
}

float int2float(int v) {
    return float(v) / float(INTERVALL);
}

int float2int(float v) {
    return int( v * float(INTERVALL) );
}

//fonction d'affichage de la matrix data_2
void compute_affichage(uint x, uint y, uint p) {
    // Mappage lineaire de data_0[p] a l'intervalle [0, INTERVALL]
    float mappedValue = int2float( data_0[p] );//pour avoir un chiffre entre 1 et 0
	vec4 resultColor;

	//en fonction de la valeur utilise des fonction diffrente pour melanger differente couleur
	if(mappedValue > 0.66f){
		mappedValue = 3*mappedValue -2;
	    resultColor = mix(MID_ALIVE_COLOR,ALIVE_COLOR,mappedValue);
	}else if(mappedValue > 0.33f){
		mappedValue = 3*mappedValue -1;
		resultColor = mix(BELLOW_MID_ALIVE_COLOR,MID_ALIVE_COLOR,mappedValue);
	}else{
		mappedValue = 3*mappedValue;
		resultColor = mix(DEAD_COLOR,BELLOW_MID_ALIVE_COLOR,mappedValue);
	}

    // Convertir chaque composante en entier et les combiner
    int resultInt = int(resultColor.a * 255.0f) << 24 |
                    int(resultColor.b * 255.0f) << 16 |
                    int(resultColor.g * 255.0f) << 8 |
                    int(resultColor.r * 255.0f);

    // Affecter le resultat a data_2[p]
    data_2[p] = resultInt;
	
}


float gauss(float x, float mu, float sigma) {
    return exp(-0.5f * pow((x - mu) / sigma, 2.0f));
}

float calculateDistance(float x, float y) {
    // Vous pouvez ajuster le centre du filtre en changeant les valeurs 1.0f
    return sqrt((1.0f - x) * (1.0f - x) + (1.0f - y) * (1.0f - y));
}


//fonction de croissance pour test de game of life n'est pas utilise actuellement 
float growth_GoL(float u) {
    bool mask1 = (u >= 1.0f) && (u <= 3.0f);
    bool mask2 = (u > 3.0f) && (u <= 4.0f);
    
    return -1.0f + (u - 1.0f) * float(mask1) + 8.0f * (1.0f - u/4.0f) * float(mask2);
}

//fonction de croissance
float growth_lenia(float u) {
    float mu = 0.15f;
    float sigma =0.015f;
    return -1.0f + 2.0f * gauss(u, mu, sigma);
}

void compute_next_step(uint x, uint y, uint p,float K[R * 2 + 1][R * 2 + 1]) {
		float nmax = 0;
		float somme_k = 0;
		if ( x > 0 && y > 0 && x < WSX-1 && y < WSY - 1) { // in sandbox
        	float n = 0.0f; // Somme des voisins ponderee
        	for (int i = -R; i <= R; ++i) {
            	for (int j = -R; j <= R; ++j) {
                	uint ix = x + i;
                	uint jy = y + j;
                	if (ix >= 0 && jy >= 0 && ix < WSX && jy < WSY) { // Verifier les limites du bac a sable
                	    uint k = ix + jy * WSX;
                    	if (k != p){
						    float nb =  float(  int2float(data_0[k] )        ) * K[i + R][j + R];
                    	    n += nb;
							somme_k = somme_k + K[i + R][j + R];
						}
                	}
            	}
        	}
			if(somme_k != 0){
				//n = n / somme_k;
			}
        	float taux_croissance = growth_lenia(n);
        	//data_1[p] = int(clamp(float(data_0[p]) + ((taux_croissance *0.1f) * float(INTERVALL)), 0.0f, float(INTERVALL)));
            float new_croissance = taux_croissance * 0.1f ;

            float old_value = int2float(data_0[p]) ;
            float new_value = clamp(new_croissance + old_value    ,0.0f,  1.0f );

            int new_value_int = float2int(new_value) ;

            data_1[p] = new_value_int ;
		}
}

void main() {
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint p = x + y * WSX;
	vec2 center = vec2(WSX/2, WSY/2);  // Coordonnees du premier point
	vec2 point2 = vec2(x,y);  // Coordonnees du deuxieme point

	float K[R * 2 + 1][R * 2 + 1];

	if ( step == 0 ) { // initialisation ************************
		if ( current_pass == 0 ) {

			int ytab[R * 2 + 1];
			int xtab[R * 2 + 1];

			float distance[R * 2 + 1][R * 2 + 1];


			float mu = 0.15f;
    		float sigma =0.015f;

    		for (int i = -R; i <= R; ++i) {
        		xtab[i + R] = i;
        		ytab[i + R] = i;
    		}
			
    		for (int i = -R; i <= R; ++i) {
        		for (int j = -R; j <= R; ++j) {
            		distance[i + R][j + R] = sqrt(float(i * i + j * j)) / float(R);
        		}
    		}

    		for (int i = -R; i <= R; ++i) {
        		for (int j = -R; j <= R; ++j) {
            		K[i + R][j + R] = gauss(distance[i + R][j + R], mu, sigma);
        		}
    		}

			// Couper les valeurs de K a d=1
			for (int i = -R; i <= R; ++i) {
			    for (int j = -R; j <= R; ++j) {
			        if (distance[i + R][j + R] > 1.0f) {
			            K[i + R][j + R] = 0.0f;
			        }
			    }
			}

			// Trouver la valeur maximale dans le tableau K
			float maxK = 0.0f;
			for (int i = -R; i <= R; ++i) {
			    for (int j = -R; j <= R; ++j) {
			        if (K[i + R][j + R] > maxK) {
			            maxK = K[i + R][j + R];
			        }
			    }
			}

			// Normaliser le tableau K
			for (int i = -R; i <= R; ++i) {
			    for (int j = -R; j <= R; ++j) {
			        K[i + R][j + R] = K[i + R][j + R] / maxK;
			    }
			}
			
			data_1[p] = DEAD;
			data_2[p] = DEAD;
			if(INIT == 0){
				data_0[p] =  int(random(vec2(x, y))*float(INTERVALL)) ;
			}else if(INIT == 1){
				float distanceP = length(point2 - center);
				if(distanceP < float(R)){
					data_0[p] = int(INTERVALL * (float(R) - distanceP));
				}else{
					data_0[p] = DEAD;
				}
			}else if(INIT == 2){

				    if(x<20 || x>=40 || y<20 || y>=40){
							data_0[p] = DEAD;
					}else{
							data_0[p] = float2int(orbium[x-20][y-20]);
					}
			}
			

			

		}
	} else { // in process *********************************
		if (current_pass == 0){
			compute_next_step(x, y, p, K);
		}
        if (current_pass == 1){
			data_0[p] = data_1[p]; 
        }
        if (current_pass == 2){
			compute_affichage(x,y,p);
		}
	}
}

