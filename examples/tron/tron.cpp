
//  800 * 450
//  data_0 : affichage
//  data_1 : motos
//  data_2 : faisceaux motos

/*
 * Couleurs Motos :
 * Rouge : 0xFF0000FF
 * Bleu : 0xFF00FF00
 * Vert : 0xFFFF0000
*/

/*
 * Couleurs faisceaux :
 * Rouge modifie: 0xFF0055FF
 * Bleu modifie: 0xFF55FF00
 * Vert modifie: 0xFFFF0055
*/


#define Init 0
#define InitMotorcycles 1
#define InitBeams 2


#define Display data_0

#define Motorcycles data_1

#define Beams data_2





//fonction random
float random (ivec2 uv) {
    return fract(sin(dot(uv.xy,
        vec2(12.9898f,78.233f))) * 43758.5453123f);
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    uint p = pos.x + pos.y * WSX;

    if(step == Init)
    {
        Display[p] = 0xFF000000;
        Motorcycles[p] = 0xFF000000;
        Beams[p] = 0xFF000000;
    }
    if(step == InitMotorcycles)
    {
        if(pos.x >= 195 && pos.x < 205 && pos.y >= 195 && pos.y < 205)
            Motorcycles[p] = 0xFF0000FF; // rouge
        
        if(pos.x >= 395 && pos.x < 405 && pos.y >= 195 && pos.y < 205)
            Motorcycles[p] = 0xFF00FF00; // vert
        
        if(pos.x >= 595 && pos.x < 605 && pos.y >= 195 && pos.y < 205)
            Motorcycles[p] = 0xFFFF0000; // bleu
        
        Display[p] += Motorcycles[p];
    }
    if(step == InitBeams)
    {
        if(pos.x >= 198 && pos.x < 202 && pos.y >= 180 && pos.y < 195)
            Beams[p] = 0xFF0055FF; // rouge modifie
        if(pos.x >= 398 && pos.x < 402 && pos.y >= 180 && pos.y < 195)
            Beams[p] = 0xFF55FF00; // vert modifie
        if(pos.x >= 598 && pos.x < 602 && pos.y >= 180 && pos.y < 195)
            Beams[p] = 0xFFFF0055; // bleu modifie

        Display[p] += Beams[p];
    }

    if(step == 3){
        if(Motorcycles[p] == 0xFF0000FF)
        {
            Motorcycles[(pos.x+10) + (pos.y * WSX)] = 0xFF0000FF;
        }
    }
    if(step == 4){
        Display[p] += Motorcycles[p];
    }

}