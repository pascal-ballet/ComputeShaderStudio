void main() {

    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y*WSX;

    if (step == 0) // init
        data_0[p] = int(0xFF0000FF - y) ; // Fond
    else {
        float dx = float(mousex) - float(x) ;
        float dy = float(mousey) - float(y) ;
        float dist = sqrt( dx*dx + dy*dy );

        if (dist < 10) {
            data_0[p] = 0xFF00FF00; // Cercle vert
        } else {
            data_0[p] = int(0xFF0000FF - y) ; // Fond
        }

    }
}