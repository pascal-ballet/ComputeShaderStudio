
//  800 * 450
//  data_0 : affichage
//  data_1 : motos
//  data_2 : faisceaux motos

/*
 * Couleurs Motos :
 * Rouge : 0xFF0000FF
 * Bleu : 0xFFFF0000
 */

/*
 * Couleurs faisceaux :
 * Rouge modifie: 0xFF0055FF
 * Bleu modifie: 0xFF55FF00
 */

#define CLEAR 0xFF000000
#define MOTO_1 0xFF0000FF
#define MOTO_2 0xFFFF0000

#define Init 0
#define InitMotorcycles 1
#define InitBeams 2

#define Speed 30

#define Display data_0

#define Motorcycles data_1

#define Beams data_2

const uint dimMoto = 10;

const uint dir_haut = 0;
const uint dir_droite = 1;
const uint dir_bas = 2;
const uint dir_gauche = 3;

const int directions[4] = int[](0, 1, 2, 3);

void moveMotorcyle(int id_moto, uint dir)
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    uint p = pos.x + pos.y * WSX;

    if (step % Speed == 0)
    {
        if (Motorcycles[p] == id_moto)
        {
            Motorcycles[p] = CLEAR;
            Display[p] = CLEAR;
            if (dir == dir_haut)
                Motorcycles[(pos.x) + ((pos.y - dimMoto) * WSX)] = id_moto;
            if (dir == dir_droite)
                Motorcycles[(pos.x + dimMoto) + (pos.y * WSX)] = id_moto;
            if (dir == dir_bas)
                Motorcycles[(pos.x) + ((pos.y + dimMoto) * WSX)] = id_moto;
            if (dir == dir_gauche)
                Motorcycles[(pos.x - dimMoto) + (pos.y * WSX)] = id_moto;
        }
    }
    if (step % Speed == 1)
    {
        Display[p] = Motorcycles[p] + Beams[p];
    }
}

// fonction random
float random(vec2 uv)
{
    return fract(sin(dot(uv.xy,
                         vec2(12.9898f, 78.233f))) *
                 43758.5453123f);
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    uint p = pos.x + pos.y * WSX;

    if (step == Init)
    {
        Display[p] = CLEAR;
        Motorcycles[p] = CLEAR;
        Beams[p] = CLEAR;
    }
    if (step == InitMotorcycles)
    {
        if (pos.x >= 195 && pos.x < 205 && pos.y >= 195 && pos.y < 205)
            Motorcycles[p] = MOTO_1; // rouge

        if (pos.x >= 595 && pos.x < 605 && pos.y >= 195 && pos.y < 205)
            Motorcycles[p] = MOTO_2; // bleu

        Display[p] += Motorcycles[p];
    }
    if (step == InitBeams)
    {
        /*if (pos.x >= 198 && pos.x < 202 && pos.y >= 180 && pos.y < 195)
            Beams[p] = 0xFF0055FF; // rouge modifie
        if (pos.x >= 598 && pos.x < 602 && pos.y >= 180 && pos.y < 195)
            Beams[p] = 0xFFFF0055; // bleu modifie

        Display[p] += Beams[p];*/
    }

    // moveMotorcyle(MOTO_1, directions[int(step * random(vec2(10., 10.))) % 4]);

    float choix_direction = random(vec2(float(step), float(step)));

    if (choix_direction < 0.25)
        moveMotorcyle(MOTO_1, dir_haut);
    else if (choix_direction < 0.50)
        moveMotorcyle(MOTO_1, dir_droite);
    else if (choix_direction < 0.75)
        moveMotorcyle(MOTO_1, dir_bas);
    else
        moveMotorcyle(MOTO_1, dir_gauche);
}