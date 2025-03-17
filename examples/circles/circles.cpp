
#define RADIUS 40

void main()
{
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);

    // initial background is black & opac
    if (step == 0) {
        data_0[p] = 0xFF000000 + y;
    } else { // draw a fading red circle
        float dx = float(mousex - x);
        float dy = float(mousey - y);
        float dist = sqrt(dx*dx+dy*dy);
        int pix = data_0[p];
        int col = pix & 0x00FFFFFF ;
        float r = RADIUS + 10*sin(step/10.0f);
        if( dist < r-1 ) { // interior
            col = col + 0x04 ;
            data_0[p] = 0xFF000000 + col ;
        }
        if( dist >= r-1 && dist <= r+1 ) { // perimeter
            data_0[p] = 0xFF0000FF ;
        }
        if( dist > r + 1) { // ouside
            if (col > 0)
                data_0[p] = pix - 1;
        }
    }
}