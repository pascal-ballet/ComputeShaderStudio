
// Write your code HERE
int mouseClicked = 0;


void main() {

	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    //initialisation
    if(step == 0){
        if(x>50 && x<70 && y>50 && y<80){
        data_0[p] = 0xFF00FF00; //fluide initial
        }
        else if(y>120){
        data_0[p] = 0xFFFFFFFF; //sol
        }
        else if(x>125 || x < 3){
            data_0[p] = 0xFFFFFFFF;
        }
        else{
            data_0[p] = 0x00000000; //backround
        }
    }
    //boucle principale
    else{
        //mouse left
        if(x == mousex && y==mousey && mouse_button == 1){
            data_0[p] = 0xFFFFFFFF;
            data_0[p+1] = 0xFFFFFFFF;
            data_0[p-1] = 0xFFFFFFFF;
            data_0[p+WSX] = 0xFFFFFFFF;
            data_0[p+WSX+1] = 0xFFFFFFFF;
            data_0[p+WSX-1] = 0xFFFFFFFF;
        }
        if(x == mousex && y==mousey && mouse_button == 0){
            data_0[p] = 0xFF0000FF;
        }
        //mouse left
        if(x == mousex && y==mousey && mouse_button == 2){
            data_0[p] = 0xFF00FF00;
        }
        //both at the same time
        if(x == mousex && y==mousey && mouse_button == 3){
            data_0[p] = 0xFFFF0000;
        }
        //si le pixel est non vide et non sol
        if(data_0[p] != 0xFFFFFFFF && data_0[p]!= 0x00000000){
            //il tombe
            if(data_0[p+WSX] == 0x00000000){
               data_0[p+WSX] = data_0[p];
               data_0[p] =  0x00000000;
            }
             //l'eau va sur les cotes
             
              //vide a gauche en bas
             else if(data_0[p-1+WSX] == 0x00000000){
                data_0[p-1+WSX] = data_0[p];
                data_0[p] = 0x00000000;
             }

             //vide a droite en bas
             else if(data_0[p+1+WSX] == 0x00000000){
                data_0[p+1+WSX] = data_0[p];
                data_0[p] = 0x00000000;
             }

             //il y a un pixel en bas/gacuhe/droit
             else{
                if(y<120){

                    if(data_0[p]!=0xFFFFFFFF && data_0[p+WSX]!=0xFFFFFFFF && data_0[p+WSX+1]!=0xFFFFFFFF && data_0[p+WSX-1]!=0xFFFFFFFF ){

                        int rouge1 =(data_0[p] & 0x000000FF);
                        int green1 =((data_0[p] & 0x0000FF00)>>8);
                        int blue1 = (data_0[p] & 0x00FF0000)>>16;

                        int rouge2 =(data_0[p+WSX] & 0x000000FF);
                        int green2 =((data_0[p+WSX] & 0x0000FF00)>>8);
                        int blue2 = (data_0[p+WSX] & 0x00FF0000)>>16;

                        int rouge3 =(data_0[p+WSX-1] & 0x000000FF);
                        int green3 =((data_0[p+WSX-1] & 0x0000FF00)>>8);
                        int blue3 = (data_0[p+WSX-1] & 0x00FF0000)>>16;

                        int rouge4 =(data_0[p+WSX+1] & 0x000000FF);
                        int green4 =((data_0[p+WSX+1] & 0x0000FF00)>>8);
                        int blue4 = (data_0[p+WSX+1] & 0x00FF0000)>>16;

                        int rouge = (rouge1 + rouge2 + rouge3 + rouge4)/4;
                        int green = (green1 + green2 + green3 + green4)/4;
                        int blue = (blue1 + blue2 + blue3 + blue4)/4;


                        int couleur = (0xFF << 24) + (blue<<16) + (green << 8)  +rouge ; 

                        data_0[p+WSX] = couleur;
                        data_0[p] = couleur;
                        
                        ///??
                        //if(sin(step/30) >= 0){
                            data_0[p+WSX+1] = couleur;
                            data_0[p+WSX-1] = couleur;
                        //}else{
                            data_0[p+WSX-1] = couleur;
                            data_0[p+WSX+1] = couleur;
                        //}



                    }

                    
                    
                    
                    
                }
                
             }

             

          
           
        }

        

    }

    


	//
	//data_0[p] = 0xFFF00FFF - int(p)*(step+1);



}
