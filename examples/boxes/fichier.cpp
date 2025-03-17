// Adapte de https://www.shadertoy.com/view/wc2XRh
// Version optimisee pour la bibliotheque Compute Shader Studio dans Godot.
#define POINTCOUNT 8
#define PI 3.14159
#define iResolution vec2(float(WSX), float(WSY))
#define inBox(low, high, point) (low.x <= point.x && low.y <= point.y && high.x > point.x && high.y > point.y)
#define toUv(point) (2.0 * (point - 0.5 * iResolution) / iResolution.y)

void main()
{
    // Recuperer les coordonnees du pixel
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    // Calculer l'indice lineaire dans data_0
    uint p = x + y * WSX;

    // Convertir les coordonnees du pixel en coordonnees uv
    vec2 fragCoord = vec2(x, y);
    vec2 uv = toUv(fragCoord);
    // Determiner les bornes inferieure et superieure de la cellule
    vec2 low = floor(uv);
    vec2 high = ceil(uv);
    // Restriction de uv dans la cellule
    uv = mod(uv, vec2(1.0));

    // Calculer le temps a partir de la variable step
    float t = float(step) * 0.005;

    // Creer un tableau de POINTCOUNT points
    vec2 points[POINTCOUNT];
    for (int i = 0; i < POINTCOUNT; i++)
    {
        float fi = float(i);
        points[i] = vec2(
            (1.75 - 0.125 * fi) * cos(1.333 * t - (1.0 - 0.2 * t) * fi / PI),
            (1.0 - 0.1 * fi) * sin((fi + 1.0) * t + PI * 0.5)
        );
    }

    // Boucle de subdivision pour determiner le niveau de subdivision
    int iters = 0;
    while (iters < 8)
    {
        iters++;
        bool sub = false;
        // Verifier si un point se trouve dans la cellule
        for (int i = 0; i < POINTCOUNT; i++)
        {
            if (inBox(low, high, points[i]))
            {
                sub = true;
                break;
            }
        }
        if (sub)
        {
            // Calculer le centre de la cellule
            vec2 center = (high + low) * 0.5;
            // Subdivision horizontale
            if (uv.x > 0.5)
            {
                low.x = center.x;
                uv.x = uv.x * 2.0 - 1.0;
            }
            else
            {
                high.x = center.x;
                uv.x *= 2.0;
            }
            // Subdivision verticale
            if (uv.y > 0.5)
            {
                low.y = center.y;
                uv.y = uv.y * 2.0 - 1.0;
            }
            else
            {
                high.y = center.y;
                uv.y *= 2.0;
            }
        }
        else
        {
            break;
        }
    }

    // Calculer une mesure "m" de la distance par rapport au centre de la cellule
    float m = max(abs(uv.x - 0.5), abs(uv.y - 0.5)) * 2.0;
    // "s" est une valeur lisse en fonction du nombre d'iterations
    float s = smoothstep(1.0 - 0.01 * pow(2.0, float(iters)), 1.0, m);
    // Creer une couleur en niveaux de gris
    vec3 col = vec3(s);

    // Conversion de la couleur en valeurs entieres (0 a 255)
    int a = 255;
    int r = int(clamp(col.r, 0.0, 1.0) * 255.0);
    int g = int(clamp(col.g, 0.0, 1.0) * 255.0);
    int b = int(clamp(col.b, 0.0, 1.0) * 255.0);

    // Stocker la couleur dans data_0 en format ARGB
    data_0[p] = (a << 24) | (r << 16) | (g << 8) | b;
}