// Write your code HERE
void main() {
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint p = x + y * WSX;
	data_0[p] = 0xFFFF0000 - int(x+y*1024)*(step);
}
