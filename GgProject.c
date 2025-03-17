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
	// + définition du fond marron
	int opacite =255;
	// int r = 92;
	// int g = 64;
	// int b = 51;

	int r = 0;
	int g = 0;
	int b = 0;
	int i=0;

	//projet principale 
	#if 1
	{

		//définition de la dimension de chaque case
		int dimension = (130/10);

		float cx = (x / dimension) * dimension + dimension / 2;
		float cy = (y / dimension) * dimension + dimension / 2;

	
			if (((x / dimension) + (y / dimension)) % 2 == 0) {
				// Case blanche

				float dx = cx - float(x);
				float dy= cy - float(y);
				float dist=sqrt(dx*dx +dy*dy);

				//définition des pions noirs
				if( (dist<(dimension/2)) && ( (y / dimension) < 3 )){
					r = 0;
					g = 0;
					b = 0;
				}
				//définition des pions blanc
				else if( (dist<(dimension/2)) && (  (y / dimension) >= 7 ) ){
					r = 128;
					g = 128;
					b = 128;
				}
				else{
					r = 255;
					g = 255;
					b = 255;
				}
			}  
			else {
				// Case marron
				float dx = float(cx) - float(x);
				float dy= float(cy) - float(y);
				float dist=sqrt(dx*dx +dy*dy);

				//définition des pions noirs
				if( (dist<(dimension/2)) && ( (y / dimension) < 3 )){
					r = 0;
					g = 0;
					b = 0;
				}
				//définition des pions blanc
				else if( (dist<(dimension/2)) && (  (y / dimension) >= 7 ) ){
					r = 128;
					g = 128;
					b = 128;
				}
				else{
					r = 92;
					g = 64;
					b = 51;
				}
			}

			if(mouse_button == 1){}
		

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
