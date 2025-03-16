
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
#define BEAM_1 0xFF0055FF
#define BEAM_2 0xFF55FF00

#define Display data_0

#define Motorcycles data_1

#define Beams data_2

#define Init 0

const int dimMoto = 10;
// fonction random
float random(vec2 uv)
{
    return fract(sin(dot(uv.xy + random_time * 0.00001,
                         vec2(12.9898f, 78.233f))) *
                 43758.5453123f);
}

void InitGame(ivec2 pos)
{
    uint p = pos.x + pos.y * WSX;
    Display[p] = CLEAR;
    Motorcycles[p] = CLEAR;
    Beams[p] = CLEAR;

    if (pos.x >= 300 - dimMoto && pos.x < 290 + dimMoto && pos.y >= 200 - dimMoto && pos.y < 190 + dimMoto)
        Motorcycles[p] = MOTO_1;

    if (pos.x >= 600 - dimMoto && pos.x < 590 + dimMoto && pos.y >= 200 - dimMoto && pos.y < 190 + dimMoto)
        Motorcycles[p] = MOTO_2;

    Display[p] += Motorcycles[p];
}
bool isDirectionClear(ivec2 basePos, int direction)
{
    ivec2 nextPos = basePos;

    switch (direction)
    {
    case 0:
        nextPos.y -= dimMoto;
        break; // haut
    case 1:
        nextPos.x += dimMoto;
        break; // droite
    case 2:
        nextPos.y += dimMoto;
        break; // bas
    case 3:
        nextPos.x -= dimMoto;
        break; // gauche
    }

    // check limites
    if (nextPos.x < 0 || nextPos.x + dimMoto > WSX ||
        nextPos.y < 0 || nextPos.y + dimMoto > WSY - 2)
    {
        return false;
    }

    // check si prochain emplacement déja rempli
    for (int j = 0; j < dimMoto; j++)
    {
        for (int i = 0; i < dimMoto; i++)
        {
            ivec2 checkPos = nextPos + ivec2(i, j);
            uint checkP = checkPos.x + checkPos.y * WSX;

            if (Motorcycles[checkP] != CLEAR || Beams[checkP] != CLEAR)
            {
                return false;
            }
        }
    }

    return true;
}

int chooseDirection(ivec2 basePos, int id_moto, int randomDir)
{
    // si direction aléatoire disponible alors go
    if (isDirectionClear(basePos, randomDir))
    {
        return randomDir;
    }

    // si direction aléatoire non disponible alors tenter toutes les autres
    float seed = random(vec2(float(step) * 0.5678f + float(id_moto), step));
    int startDir = int(seed * 4.0);

    for (int i = 0; i < 4; i++)
    {
        int dir = (startDir + i) % 4;
        if (dir != randomDir && isDirectionClear(basePos, dir))
        {
            return dir;
        }
    }
    // si on trouve pas une direction valide
    return -1;
}

void moveMotorcyle(int id_moto)
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    uint p = pos.x + pos.y * WSX;

    if (Motorcycles[p] != id_moto)
        return;

    ivec2 basePos = ivec2((pos.x / dimMoto) * dimMoto, (pos.y / dimMoto) * dimMoto);
    if (pos.x != basePos.x || pos.y != basePos.y)
        return; // check coin supérieur gauche

    // choix direction aléatoire
    float seed = random(vec2(float(step) * (id_moto == MOTO_1 ? 0.125445f : 0.68419f), step)); // choix arbitraire des valeurs, peut largement mieux faire
    int randomDir = int(seed * 4.0f); // 0: haut, 1: droite, 2: bas, 3: gauche

    int direction = chooseDirection(basePos, id_moto, randomDir);

    // si on trouve une direction valide
    if (direction >= 0)
    {
        // calcul nouvelle position
        ivec2 newPos = basePos;
        switch (direction)
        {
        case 0:
            newPos.y -= dimMoto;
            break; // haut
        case 1:
            newPos.x += dimMoto;
            break; // droite
        case 2:
            newPos.y += dimMoto;
            break; // bas
        case 3:
            newPos.x -= dimMoto;
            break; // gauche
        }

        // efface avant d'avancer
        for (int j = 0; j < dimMoto; j++)
        {
            for (int i = 0; i < dimMoto; i++)
            {
                ivec2 oldPos = basePos + ivec2(i, j);
                uint oldP = oldPos.x + oldPos.y * WSX;
                Motorcycles[oldP] = CLEAR;
                Beams[oldP] = (id_moto == MOTO_1) ? BEAM_1 : BEAM_2;
            }
        }

        // la moto avance
        for (int j = 0; j < dimMoto; j++)
        {
            for (int i = 0; i < dimMoto; i++)
            {
                ivec2 newPixelPos = newPos + ivec2(i, j);
                uint newP = newPixelPos.x + newPixelPos.y * WSX;
                Motorcycles[newP] = id_moto;
            }
        }
    }
}

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    uint p = pos.x + pos.y * WSX;

    int slowFactor = 10;

    if (step == Init)
    {
        InitGame(pos);
    }

    if (step > 1 && step % slowFactor == 0)
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
    if (step > 1 && step % slowFactor == 1)
    {
        Display[p] = Motorcycles[p] + Beams[p];
    }
}