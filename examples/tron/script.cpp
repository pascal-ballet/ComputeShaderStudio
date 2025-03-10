#define MORT 0xFFFFFFFF
#define VIVANT 0xFF000000

// shared uint incX;

/*layout(set = 0, binding = 0) buffer incBuffer
{
    uint incX;
    uint incY;
};*/

struct color
{
    int r;
    int g;
    int b;
};

int rgb_to_int(color c)
{
    return 0xFF000000 | int(c.r) | int(c.g) << 8 | int(c.b) << 16;
}

void dessinerSphere(uint centerX, uint centerY, float radius, color c)
{
    vec2 center = vec2(centerX, centerY);

    // Coordonnees du pixel actuel
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

    // Calculer la position du pixel
    uint p = pos.x + pos.y * WSX;

    // Calculer la distance au carre entre le pixel et le centre du cercle
    vec2 delta = pos - center;
    float distanceCarre = dot(delta, delta);

    // Si le pixel est a l'interieur du cercle
    if (distanceCarre <= radius * radius)
    {
        data_0[p] = rgb_to_int(c); // ROUGE
    }
}

void main()
{

    // Coordonnees du pixel actuel
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    uint p = pos.x + pos.y * WSX;

    int rayon = 50;
    color c = {255, 0, 132};   // bgra
    color c1 = {110, 50, 150}; // bgra
    color c2 = {255, 25, 87};  // bgra
    color c3 = {30, 0, 16};    // bgra

    /*
    if (rayon + step + rayon < 800)
    {
        dessinerSphere(rayon + incX, rayon, rayon, c);
        incX++;
    }
*/

    // data_0[p] = MORT;

    // dessinerSphere( uint(mousex), uint(mousey) , rayon, c1);

    data_0[p] = MORT;

    if ((mousex >= 0) || (mousey >= 0))
    {
        dessinerSphere(mousex, mousey, rayon, c1);
    }

    /*
    if(pos.x < 400 && pos.y < 225)
    {
        dessinerSphere(  incX*2, incX , incX, c);
    }
    else
    {
        dessinerSphere(  incX*2, incX , 40 , c);
    }

    for(uint i = 0; i < 800; i+=100)
    {
        for (uint j = 0; j < 450; j+=50)
        {
            dessinerSphere( i , j , rayon, c1);
        }
    }
    */
    /*
    dessinerSphere( 150 , 50 , rayon, c1);
    dessinerSphere( 250 , 50 , rayon, c2);
    dessinerSphere( 350 ,  50 , rayon, c3);
    dessinerSphere( 450 , 50,  rayon, c);
    dessinerSphere( 550 ,  50 , rayon, c1);
    dessinerSphere( 650 , 50 , rayon, c2);
    dessinerSphere( 750 ,  50 , rayon, c3);

    dessinerSphere( 100 ,   incX , rayon*2, c1);
    dessinerSphere( 300 ,   200 , rayon*2, c2);
    */
    // incX++;

    /*else
    {
        dessinerSphere(rayon + incX, rayon, rayon, c);
        incX--;
    }
    */
    /*
     if (incY + rayon < 450 )
     {
         dessinerSphere(rayon, rayon + incY, rayon, c);
         incY++;
     }*/
    // Dessiner la sphere pour chaque pixel
}