void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
    // Logique de calcul du shader
    data_0[p] = 0xFF000000;  // Exemple : pixel noir
}