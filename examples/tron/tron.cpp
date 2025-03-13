
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
#define BEAM 0xFF00FF00

#define Display data_0

#define Motorcycles data_1

#define Beams data_2

#define Init 0
#define InitMotorcycles 1

#define Speed 5

const int dimMoto = 10;

// fonction random
float random(vec2 uv)
{
    return fract(sin(dot(uv.xy,
                         vec2(12.9898f, 78.233f))) *
                 43758.5453123f);
}


void InitGame()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    uint p = pos.x + pos.y * WSX;
    Display[p] = CLEAR;
    Motorcycles[p] = CLEAR;
    Beams[p] = CLEAR;
}

ivec2 getNewPosition(ivec2 pos, int id_moto)
{
    ivec2 newPos = pos;

    float choix_direction;
    if (id_moto == MOTO_1)
    {
        choix_direction = random(vec2(float(step), float(current_pass * sin(MOTO_1))));
    }
    else if (id_moto == MOTO_2)
    {
        choix_direction = random(vec2(float(step), float(current_pass * sin(MOTO_2))));
    }

    if (choix_direction < 0.25) // haut
        newPos.y -= dimMoto;
    else if (choix_direction < 0.50) // droite
        newPos.x += dimMoto;
    else if (choix_direction < 0.75) // bas
        newPos.y += dimMoto;
    else if (choix_direction < 1.00) // gauche
        newPos.x -= dimMoto;

    return newPos;
}

void moveMotorcyle(int id_moto)
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    uint p = pos.x + pos.y * WSX;

    if (step % Speed == 0)
    {
        ivec2 newPos = getNewPosition(pos, id_moto);

        if (newPos.x >= 0 && newPos.x < WSX && newPos.y >= 0 && newPos.y < WSY - 2)
        {
            if (Motorcycles[newPos.x + newPos.y * WSX] == CLEAR)
            {
                Motorcycles[p] = BEAM;
                Display[p] += Motorcycles[p];

                Motorcycles[newPos.x + newPos.y * WSX] = id_moto;
            }
            else if (Motorcycles[newPos.x + newPos.y * WSX] == BEAM)
            {
                
                uint cpt_beam = 0;
                ivec2 finPos = getNewPosition(pos, id_moto);
                if (finPos == newPos || Motorcycles[finPos.x + finPos.y * WSX] == BEAM)
                {
                    cpt_beam++;
                    if (cpt_beam == 4)
                    {
                        InitGame();
                    }
                    finPos = getNewPosition(pos, id_moto);
                }

                Motorcycles[p] = BEAM;
                Display[p] += Motorcycles[p];

                Motorcycles[newPos.x + newPos.y * WSX] = id_moto;
            }
        }
    }
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    uint p = pos.x + pos.y * WSX;

    if (step == Init)
    {
        InitGame();
    }
    if (step == InitMotorcycles)
    {

        if (pos.x >= 300 - dimMoto && pos.x < 290 + dimMoto && pos.y >= 200 - dimMoto && pos.y < 190 + dimMoto)
            Motorcycles[p] = MOTO_1; // bleu

        if (pos.x >= 600 - dimMoto && pos.x < 590 + dimMoto && pos.y >= 200 - dimMoto && pos.y < 190 + dimMoto)
            Motorcycles[p] = MOTO_2; // bleu

        Display[p] += Motorcycles[p];
    }

    float choix_direction;

    if (step > 1 && step % 2 == 0)
    {
        if (Motorcycles[p] == MOTO_1)
        {
            moveMotorcyle(MOTO_1);
        }
        else if (Motorcycles[p] == MOTO_2)
        {
            moveMotorcyle(MOTO_2);
        }
    }
    if (step > 1 && step % 2 == 1)
    {
        Display[p] = Motorcycles[p] + Beams[p];
    }
}