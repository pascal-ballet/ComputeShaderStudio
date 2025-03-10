#define MORT 0xFFFFFFFF







void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    uint p = pos.x + pos.y * WSX;







    data_0[p] = MORT;


}