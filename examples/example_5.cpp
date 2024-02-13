// Write your code HERE
void main() {
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint p = x + y * WSX;
	int dx = 64 - int(x);
	int dy = 64 - int(y);
	int r = int(64.0*sin(step/100.0));
	int d = int(dx*dx+dy*dy) ;
	data_0[p] = 0xFF000000 - 1500* (d + step) ;
}
