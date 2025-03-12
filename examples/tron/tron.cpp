
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

#define Display data_0

#define Motorcycles data_1

#define Beams data_2
#define Beams_id data_3

#define Init 0
#define InitMotorcycles 1

#define Speed 30

const uint dimMoto = 10;

// fonction random
float random(vec2 uv)
{
    return fract(sin(dot(uv.xy,
                         vec2(12.9898f, 78.233f))) *
                 43758.5453123f);
}


void moveMotorcyle(int id_moto, float choix_direction)
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    uint p = pos.x + pos.y * WSX;

    if (step % Speed == 0)
    {

        bool res = false;
        if (choix_direction < 0.25) // haut
        {
            res = !((pos.y - dimMoto) <= 0 || (pos.y - dimMoto) >= WSY - 2);
            if (res)
                Motorcycles[(pos.x) + ((pos.y - dimMoto) * WSX)] = id_moto;
        }
        else if (choix_direction < 0.50) // droite
        {
            res = !((pos.x + dimMoto) <= 0 || (pos.x + dimMoto) >= WSX);
            if (res)
                Motorcycles[(pos.x + dimMoto) + (pos.y * WSX)] = id_moto;
        }
        else if (choix_direction < 0.75) // bas
        {
            res = !((pos.y + dimMoto) <= 0 || (pos.y + dimMoto) >= WSY - 2);
            if (res)
                Motorcycles[(pos.x) + ((pos.y + dimMoto) * WSX)] = id_moto;
        }
        else if (choix_direction < 1.00) // gauche
        {
            res = !((pos.x - dimMoto) <= 0 || (pos.x - dimMoto) >= WSX);
            if (res)
                Motorcycles[(pos.x - dimMoto) + (pos.y * WSX)] = id_moto;
        }
        if (res)
        {
            Motorcycles[p] = CLEAR;
            Display[p] = CLEAR;
        }
    }
    if (step % Speed == 1)
    {
        Display[p] = Motorcycles[p] + Beams[p];
    }
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

    float choix_direction;

    if (Motorcycles[p] == MOTO_1)
    {
        choix_direction = random(vec2(float(step), float(MOTO_2 | 55 << 8)));
        moveMotorcyle(MOTO_1, choix_direction);
    }
    else if (Motorcycles[p] == MOTO_2)
    {
        choix_direction = random(vec2(float(step), float(MOTO_2)));
        moveMotorcyle(MOTO_2, choix_direction);
    }
}