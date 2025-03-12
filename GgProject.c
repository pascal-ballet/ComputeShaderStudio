/*
* Auteur : Gérald CALVARIN
* 
* Projet Architecture et Accélérateurs Parallèles, partie Pascal Ballet : Godot, GLSL et compute shader ( tout ça pour programmation GPU)
* projet initié le 05/03/2025
*
* Description : Ce projet va consister à faire un petit jeu de dames 
*
* Étapes : 1- faire un damier (écran noir puis noir et blanc puis cases blanches et noires)
*		   2- faire des pions (noirs et blancs )
*		   3- faire des déplacements 
*		   4- faires des actions (manger et avoir une dame)
*/


/*

	correction du prof pour un damier !

	int case_x=int(floor(x/16.0f));
	int case_y=int(floor(y/16.0f));

	if((case_x+case_y)%2==0){
		//case blanche
	}
	else{
		//case noir
	}

*/

	struct couleur { 
		int	r;
		int	g;
		int	b;
	};


void main() {
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint p = x + y * WSX;


	// définition de 4 variables o,r,g,b pour faciliter la gestion des couleurs
	int opacite =255;
	int r = 0;
	int g = 0;
	int b = 0;
	int i=0;

	//projet principale 
	#if 1
	{


		couleur plateau[128][128];

		//définition de la dimension de chaque case
		int dimension = (128/5)/2; // dimension minimale pour 4 cases
		int debutX = 0; // dimension minimale pour 4 cases
		int finX = dimension; // dimension minimale pour 4 cases
		int debutY = 0; // dimension minimale pour 4 cases
		int finY = debutY+dimension; // dimension minimale pour 4 cases

		//on à un damier de 4 cases 
		while(finX<128){
			//while(finY<128){

			if((y>=debutY) && (y<finY) && (x>=debutX) &&(x<finX)){
				plateau[x][y].r=255;
				plateau[x][y].g=255;
				plateau[x][y].b=255;

				
				r=plateau[x][y].r;
				g=plateau[x][y].g;
				b=plateau[x][y].b;
			}
					// debutY=finY+dimension;
					// finY=finY+(dimension*2);

					debutX=finX+dimension;
					finX=finX+(dimension*2);

			//}


			
		}
		
		// La scène prend les valeurs orgb définies
		data_0[p] = (opacite<<24)+(b<<16)+(g<<8)+r; 

	}
	#endif


	// base pour les pions 
	//cercle vert qui suit la sourie sur fond rouge dégradé
	#if 0 
	{
		if(step==0){
			r=255-int(y);
		}

		else{
			float dx = float(mousex) - float(x);
			float dy= float(mousey) - float(y);
			float dist=sqrt(dx*dx +dy*dy);

			if(dist<10){
				g=255;
			}
			else{
				r=255-int(y);
			}
		}
			// La scène prend les valeurs orgb définies
			data_0[p] = (opacite<<24)+(b<<16)+(g<<8)+r;
	}
	#endif
	
}




//exemples précédents ==> main séparé du projet !!!!!
#if 0
{

					int compter_voisin( uint x, uint y){
						return 3;
					}


					void main() {

						uint x = gl_GlobalInvocationID.x;
						uint y = gl_GlobalInvocationID.y;
						uint p = x + y * WSX;

						int opacite =255;
						int r = 0;
						int g = 0;
						int b = 0;

					//	Petite Animation 
						#if 0
						r = 0+int(x*(y+step));
						g = 0+int(y);
						b = 0+int(x);
						#endif

					//	Division en x ou en y de 2 couleurs fixes
						#if 0
						if(y<(128/2)){
							g = g+255;
						}
						else{
							b = b+255;
						}
						#endif

						#if 0
						//	Initialisation jeu de la vie 
							if(step==0){
								if(data_0[p]>=0){
									r = 255;
									g = 255;
									b = 255;
								}
							data_0[p] = (opacite<<24)+(b<<16)+(g<<8)+r;   //0xFF000000+(int(x)<<16) ; // - int(p)*(step+1);
							}
							else{ //jeu de la vie 
								
								int nb_v=compter_voisin(x, y);

								if((r==0) && (g==0) && (b==00)){
									if(nb_v==3){
										r = 255;
										g = 255;
										b = 255;
									}
									else{
										r = 255;
										g = 255;
										b = 255;
									}
								}

							}
							

							//data_0[p] = (opacite<<24)+(b<<16)+(g<<8)+r;   //0xFF000000+(int(x)<<16) ; // - int(p)*(step+1);
							//data_1[p] = 0xFFF00FFF ;
							//data_1[p] = 0xFF0000AA + int( 1.0 + 99999.9*sin(float(x+float(step+y))/1000.0));
						#endif
					}
}
#endif
