/*
 * Définition de la constante PI utilisée pour les calculs trigonométriques.
 */
float PI = 3.14159;

/*
 * Fonction qui retourne une matrice de rotation 2D autour de l'origine
 * en fonction d'un angle donné en radians.
 */
mat2 rotation(float rad) {
    return mat2(cos(rad), sin(rad), -sin(rad), cos(rad));
}

/*
 * Fonction qui calcule la distance signée d'un point `p` à la surface d'une sphère de rayon `s`.
 * La distance est positive à l'extérieur de la sphère et négative à l'intérieur.
 */
float sdSphere(vec3 p, float s) {
    return length(p) - s;
}

/*
 * Fonction qui encode une couleur (vec3) en un entier au format ARGB (32 bits).
 * Les composantes RGB sont clampées entre 0 et 1, puis converties en valeurs entières (0-255).
 */
int packARGB(vec3 c) {
    int R = int(clamp(c.x, 0.0, 1.0) * 255.0);
    int G = int(clamp(c.y, 0.0, 1.0) * 255.0);
    int B = int(clamp(c.z, 0.0, 1.0) * 255.0);
    int A = 255; // Alpha toujours à 255 (opaque)
    return (A << 24) | (R << 16) | (G << 8) | B;
}

void main() {
    // Récupération des coordonnées du thread en cours d'exécution
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    
    // Vérification des limites de l'écran
    if(x >= WSX || y >= WSY) return;
    
    // Calcul de l'index du pixel dans le buffer de sortie
    uint index = x + y * WSX;
    
    // Coordonnées du pixel en espace normalisé (0,0) en bas à gauche
    vec2 fragCoord = vec2(float(x), float(y));
    vec2 iResolution = vec2(float(WSX), float(WSY));
    float t = float(step) * 0.01; // Temps écoulé normalisé
    
    // Normalisation des coordonnées UV (-1 à 1 sur l'axe y)
    vec2 uv = (fragCoord - 0.5 * iResolution) / iResolution.y;
    
    // Calcul du centre de l'écran et de la direction de la souris
    vec2 screenCenter = 0.5 * iResolution;
    float mouseDir = atan(float(mousey) - screenCenter.y, float(mousex) - screenCenter.x);
    
    // Application des rotations en fonction du temps et de la souris
    float rotAngleXZ = t + mouseDir;      // Rotation dans le plan xz
    float rotAngleYZ = -t * 1.2 + mouseDir; // Rotation dans le plan yz avec un facteur d'ajustement
    
    // Initialisation de la couleur de sortie et des paramètres de la caméra
    vec3 col = vec3(0.0);
    vec3 camPos = vec3(0.0, 0.0, 3.5); // Position de la caméra
    vec3 rayDir = normalize(vec3(uv, -1.0)); // Direction du rayon normalisé
    float depth = 0.0; // Distance parcourue par le rayon
    vec3 p = vec3(0.0);
    float d = 0.0;
    
    // Boucle de lancer de rayon (Raymarching) pour estimer l'intersection avec la sphère
    for (int i = 0; i < 50; i++) {
        p = camPos + depth * rayDir; // Calcul de la position actuelle du rayon
        p.xz = rotation(rotAngleXZ) * p.xz; // Rotation sur l'axe XZ
        p.yz = rotation(rotAngleYZ) * p.yz; // Rotation sur l'axe YZ
        d = sdSphere(p, 1.25); // Calcul de la distance signée à la sphère
        depth += d; // Avancement du rayon
        if (d < 0.001 || depth > 10.0) break; // Arrêt si trop proche ou trop loin
    }
    
    // Détermination de la couleur finale selon la profondeur atteinte
    if (d > 10.0) {
        col = vec3(0.0); // Fond noir si aucune intersection
    } else {
        // Calcul des coordonnées sphériques de l'intersection
        float theta = atan(p.x, p.z);
        float phi = acos(p.y / length(p));
        
        // Paramètres de l'effet de quadrillage
        float wireDelta = PI / 18.0; // Distance entre les lignes du quadrillage
        float wireWidth = 0.02; // Largeur des lignes
        
        // Calcul de la couleur en fonction du quadrillage
        float colTheta = smoothstep(wireWidth, 0.0, abs(mod(theta, wireDelta) - wireDelta/2.0));
        float colPhi = smoothstep(wireWidth, 0.0, abs(mod(phi, wireDelta) - wireDelta/2.0));
        col += vec3(max(colTheta, colPhi));
    }
    
    // Application d'un effet de profondeur pour assombrir avec la distance
    col = mix(5.5 * col, vec3(0.0), smoothstep(0.0, 3.6, depth));
    
    // Stockage de la couleur dans le buffer de sortie
    data_0[index] = packARGB(col);
}
