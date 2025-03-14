void main() {

    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y*WSX;

	int case_x = int(floor( (x-mousex+WSX) / (150.0f*atan(3.14f*float(x+40.0f*sin(float(step)/10.0f))/float(WSX) - 3.14f ))));
	int case_y = int(floor( (y-mousey+WSY) / (150.0f*atan(3.14f*float(y+30.0f*cos(float(step)/10.0f))/float(WSY) - 3.14f ))));

    if ( (case_x + case_y)%2 == 0) // Case Blanche
        data_0[p] = int(0xFFFF00FF) ; // Fond
    else {
        data_0[p] = 0xFF00FF00; // Case Noire
    }
}
