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
#define BEAM_1 0xFF00FFFF
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

void initGame(ivec2 pos)
{
    uint p = pos.x + pos.y * WSX;
    Display[p] = CLEAR;
    Motorcycles[p] = CLEAR;
    Beams[p] = CLEAR;

    // On choisis aleatoirement une position dans la partie gauche de la simulation
    float randomY1 = random(vec2(123.45f, 67.89f)) * (WSY - 2 * dimMoto) + dimMoto;
    float randomX1 = random(vec2(234.56f, 78.90f)) * (WSX / 2 - 2 * dimMoto) + dimMoto;

    ivec2 moto1Pos = ivec2(int(randomX1), int(randomY1));

    // On choisis aleatoirement une position dans la partie droite de la simulation
    float randomY2 = random(vec2(345.67f, 89.01f)) * (WSY - 2 * dimMoto) + dimMoto;
    float randomX2 = random(vec2(456.78f, 90.12f)) * (WSX / 2 - 2 * dimMoto) + (WSX / 2);

    ivec2 moto2Pos = ivec2(int(randomX2), int(randomY2));

    // pour que les motos soit spawn alignees pour la suite des mouvements
    moto1Pos = ivec2((moto1Pos.x / dimMoto) * dimMoto, (moto1Pos.y / dimMoto) * dimMoto);
    moto2Pos = ivec2((moto2Pos.x / dimMoto) * dimMoto, (moto2Pos.y / dimMoto) * dimMoto);

    // Premiere moto
    if (pos.x >= moto1Pos.x && pos.x < moto1Pos.x + dimMoto &&
        pos.y >= moto1Pos.y && pos.y < moto1Pos.y + dimMoto)
        Motorcycles[p] = MOTO_1;

    // deuxieme moto
    if (pos.x >= moto2Pos.x && pos.x < moto2Pos.x + dimMoto &&
        pos.y >= moto2Pos.y && pos.y < moto2Pos.y + dimMoto)
        Motorcycles[p] = MOTO_2;
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

    // check si prochain emplacement deja rempli
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
    // Trouver la position de l'autre moto
    ivec2 enemyPos = ivec2(-1, -1);
    int enemyMoto = (id_moto == MOTO_1) ? MOTO_2 : MOTO_1;

    for (int y = 0; y < WSY; y++)
    {
        for (int x = 0; x < WSX; x++)
        {
            uint p = x + y * WSX;
            if (Motorcycles[p] == enemyMoto)
            {
                enemyPos = ivec2(x, y);
                break;
            }
        }
        if (enemyPos.x != -1)
            break;
    }

    // Si on trouve l'autre moto, on calcule la direction ver elle
    if (enemyPos.x != -1)
    {
        ivec2 diff = enemyPos - basePos;

        // nombre aleatoire pour decider si on suit l'ennemi ou si on fait un mouvement aleatoire
        float chaseChance = random(vec2(float(step) * 0.7234f + float(id_moto), step));

        // 70% de chance de suivre l'autre moto', 30% de faire un mouvement aleatoire
        if (chaseChance < 0.70f)
        {
            int dirTab[4];
            float dirChoice = random(vec2(float(step) * 0.3456f + float(id_moto), step));

            if (abs(diff.x) > abs(diff.y) || dirChoice > 0.5f)
            {
                dirTab[0] = (diff.x > 0) ? 1 : 3;                                   // Droite ou Gauche
                dirTab[1] = (diff.y > 0) ? 2 : 0;                                   // Bas ou Haut
                dirTab[2] = (random(vec2(step * 0.1234f, id_moto)) > 0.5f) ? 0 : 2; // Y aleatoire
                dirTab[3] = (random(vec2(step * 0.5678f, id_moto)) > 0.5f) ? 1 : 3; // X aleatoire
            }
            else
            {
                dirTab[0] = (diff.y > 0) ? 2 : 0;                                   // Bas ou Haut
                dirTab[1] = (diff.x > 0) ? 1 : 3;                                   // Droite ou Gauche
                dirTab[2] = (random(vec2(step * 0.9012f, id_moto)) > 0.5f) ? 1 : 3; // X aleatoire
                dirTab[3] = (random(vec2(step * 0.3456f, id_moto)) > 0.5f) ? 2 : 0; // Y aleatoire
            }

            // Essayer chaque direction dans l'ordre de preference
            for (int i = 0; i < 4; i++)
            {
                if (isDirectionClear(basePos, dirTab[i]))
                {
                    return dirTab[i];
                }
            }
        }
    }

    if (isDirectionClear(basePos, randomDir))
    {
        return randomDir;
    }

    // essayer toutes les directions dans un ordre aleatoire
    int essais[4] = {0, 1, 2, 3};
    for (int i = 3; i > 0; i--)
    {
        int j = int(random(vec2(float(step) * 0.8901f + float(i), id_moto)) * float(i + 1));
        int temp = essais[i];
        essais[i] = essais[j];
        essais[j] = temp;
    }

    for (int i = 0; i < 4; i++)
    {
        if (isDirectionClear(basePos, essais[i]))
        {
            return essais[i];
        }
    }

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
        return; // check coin superieur gauche

    // choix direction aleatoire
    float seed = random(vec2(float(step) * (id_moto == MOTO_1 ? 0.125445f : 0.68419f), step)); // choix arbitraire des valeurs, peut largement mieux faire
    int randomDir = int(seed * 4.0f);                                                          // 0: haut, 1: droite, 2: bas, 3: gauche

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

    int slowFactor = 2;

    if (step == Init)
    {
        initGame(pos);
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
    if (mouse_button > 0 && step > 100)
    {
        initGame(pos);
    }
}