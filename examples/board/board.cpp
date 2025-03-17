const int WIDTH = 16;
void main() {

    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y*WSX;

	int case_x = int(floor( (x / WIDTH)) ); // between 0 and WSX/16 - 1
	int case_y = int(floor( (y / WIDTH)) ); // between 0 and WSX/16 - 1

    if ( (case_x + case_y)%2 == 0) // Case Blanche
        data_0[p] = int(0xFFFF00FF) ; // Fond
    else {
        data_0[p] = 0xFF00FF00; // Case Noire
    }
}
