void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
    // Normaliser les coordonnees dans l'intervalle [0,1]
    float fx = float(x) / float(WSX);
    float fy = float(y) / float(WSY);
    
    // Calculer un effet plasma a l'aide de sinusoides et d'un deplacement temporel (step)
    float value = sin((fx + step/100.0) * 10.0)
                + sin((fy + step/100.0) * 10.0)
                + sin((fx + fy + step/100.0) * 10.0);
    
    // Normaliser la valeur obtenue (la somme varie entre -3 et 3)
    float norm = (value + 3.0) / 6.0;
    
    // Calculer les composantes de couleur sur 8 bits
    uint r = uint(255.0 * norm);
    uint g = uint(128.0 * (1.0 - norm));
    uint b = uint(255.0 * (1.0 - norm));
    uint a = 255;  // opacite maximale
    
    // Combiner les composantes en une valeur 32 bits (ARGB)
    // Le cast en int permet d'eviter l'erreur de conversion de uint vers int
    data_0[p] = int((a << 24) | (r << 16) | (g << 8) | b);
}
