#define MORT 0xFFFFFFFF
#define VIVANT 0xFF000000

layout(set = 0, binding = 0) buffer incBuffer
{
    uint incX;
    uint incY;
};
struct color
{
    int b;
    int g;
    int r;
    float a;
};

void dessinerSphere(uint centerX, uint centerY, float radius, color c)
{
    vec2 center = vec2(centerX, centerY);

    // Coordonnees du pixel actuel
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;

    if (x < WSX && y < WSY)
    {
        // Calculer la position du pixel
        uint p = x + y * WSX;

        // Calculer la distance au carre entre le pixel et le centre du cercle
        vec2 delta = vec2(x, y) - center;
        float distanceSquared = dot(delta, delta);

        // Si le pixel est a l'interieur du cercle, le marquer comme "vivant"
        if (distanceSquared <= radius * radius)
        {
            data_0[p] =
                (int(c.a) << 24) | // ALPHA
                (c.b << 16) |      // BLEU
                (c.g << 8) |       // VERT
                (c.r);             // ROUGE
        }
        else
        {
            data_0[p] = MORT;
        }
    }
}

void main()
{
    // Coordonnees du pixel actuel
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    int rayon = 50;

    color c = {132, 0, 255, 0.8f * 255.0f}; // bgra

    /*
    if (rayon + step + rayon < 800)
    {
        dessinerSphere(rayon + incX, rayon, rayon, c);
        incX++;
    }
*/

    if (incX + rayon * 2 < 800 && incX - rayon >= 0)
    {
        dessinerSphere(rayon + incX, rayon, rayon, c);
        incX++;
    }
    else
    {
        dessinerSphere(rayon + incX, rayon, rayon, c);
        incX--;
    }
    /*
     if (incY + rayon < 450 )
     {
         dessinerSphere(rayon, rayon + incY, rayon, c);
         incY++;
     }*/
    // Dessiner la sphere pour chaque pixel
}