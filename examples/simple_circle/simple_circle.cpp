void main() {

    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y*WSX;

    if (step == 0) // init
        data_0[p] = int(0xFF0000FF - y) ; // Background
    else {
        float dx = float(mousex) - float(x) ;
        float dy = float(mousey) - float(y) ;
        float dist = sqrt( dx*dx + dy*dy );

        if (dist < 10) {
            if(mouse_button == 0) // No mouse button
                data_0[p] = 0xFF00FF00; // Green circle
            else if(mouse_button == 1) // Left mouse button
                data_0[p] = 0xFFFFFF00; // Cyan circle
            else if(mouse_button == 2) // Right mouse button
                data_0[p] = 0xFF00FFFF; // Yellow circle
        } else {
            data_0[p] = int(0xFF0000FF - y) ; // Fond
        }

    }
}
