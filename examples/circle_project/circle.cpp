float PI = 3.14159;

// Fonction de rotation 2D : retourne une matrice de rotation pour un angle donné
mat2 rotation(float rad) {
    return mat2(cos(rad), sin(rad), -sin(rad), cos(rad));
}

// Fonction SDF pour une sphère : calcule la distance signée d'un point à la surface d'une sphère de rayon s
float sdSphere(vec3 p, float s) {
    return length(p) - s;
}

// Convertit un vecteur couleur (composantes entre 0 et 1) en entier ARGB 32 bits
int packARGB(vec3 c) {
    int R = int(clamp(c.x, 0.0, 1.0) * 255.0);
    int G = int(clamp(c.y, 0.0, 1.0) * 255.0);
    int B = int(clamp(c.z, 0.0, 1.0) * 255.0);
    int A = 255;
    return (A << 24) | (R << 16) | (G << 8) | B;
}

void main() {
    // Récupération des coordonnées du pixel
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    if(x >= WSX || y >= WSY) return;
    uint index = x + y * WSX;
    
    // Définition de la résolution et du pixel courant
    vec2 fragCoord = vec2(float(x), float(y));
    vec2 iResolution = vec2(float(WSX), float(WSY));
    
    // t est un facteur de temps calculé à partir de 'step'
    float t = float(step) * 0.01;
    
    // uv : coordonnées normalisées centrées sur l'écran
    vec2 uv = (fragCoord - 0.5 * iResolution) / iResolution.y;
    
    vec3 col = vec3(0.0);
    vec3 camPos = vec3(0.0, 0.0, 3.5);
    // Direction du rayon issu du pixel
    vec3 rayDir = normalize(vec3(uv, -1.0));
    float depth = 0.0;
    vec3 p = vec3(0.0);
    float d = 0.0;
    
    // Raymarching : on lance un rayon depuis la caméra et on avance jusqu'à toucher la sphère
    for (int i = 0; i < 50; i++){
        p = camPos + depth * rayDir;
        // On applique une rotation sur le plan xz de p pour faire tourner la scène
        p.xz = rotation(t) * p.xz;
        d = sdSphere(p, 1.25);
        depth += d;
        if(d < 0.001 || depth > 10.0) break;
    }
    
    // Si le rayon n'a pas touché la sphère, la couleur est noire
    if (d > 10.0) {
        col = vec3(0.0);
    } else {
        // Calcul d'un motif wireframe sur la surface de la sphère
        float theta = atan(p.x, p.z);
        float phi = acos(p.y / length(p));
        float wireDelta = PI / 18.0;
        float wireWidth = 0.02;
        float colTheta = smoothstep(wireWidth, 0.0, abs(mod(theta, wireDelta) - wireDelta/2.0));
        float colPhi = smoothstep(wireWidth, 0.0, abs(mod(phi, wireDelta) - wireDelta/2.0));
        col += vec3(max(colTheta, colPhi));
    }
    
    // Mix final avec du noir selon la profondeur pour atténuer l'effet
    col = mix(5.5 * col, vec3(0.0), smoothstep(0.0, 3.6, depth));
    data_0[index] = packARGB(col);
}