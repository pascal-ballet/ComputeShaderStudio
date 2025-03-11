#define MORT 0xFFFFFFFF
#define VIVANT 0xFF000000


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
        data_0[p] = rgb_to_int(c) + step; // ROUGE
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

    if(step == 0)
        data_0[p] = MORT;


/*
    if ((mousex >= 0) || (mousey >= 0))
    {
        dessinerSphere(mousex, mousey, rayon, c1);
    }
*/
    if(left_pressed)
        dessinerSphere(mousex, mousey, rayon, c1);
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

}