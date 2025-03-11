
void main() {
    uint pos = gl_GlobalInvocationID.x +  gl_GlobalInvocationID.y * WSX;
    int color = 0xFFFF0000;
    data_0[pos]=color;
}
