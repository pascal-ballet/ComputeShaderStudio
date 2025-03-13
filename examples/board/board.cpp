void main() {

    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y*WSX;

	int case_x = int(floor(x / 16.0f));
	int case_y = int(floor(y / 16.0f));

    if ( (case_x + case_y)%2 == 0) // Case Blanche
        data_0[p] = int(0xFFFF00FF) ; // Fond
    else {
        data_0[p] = 0xFF00FF00; // Case Noire
    }
}
