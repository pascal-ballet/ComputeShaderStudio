// Write your code HERE
void main() {
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint p = x + y * WSX;
	int dx = int(WSX)/2 - int(x);
	int dy = int(WSY)/2 - int(y);
	int r = int(64.0*sin(step/100.0));
	int d = int(dx*dx+dy*dy) ;
	data_0[p] = 0xFF000000 - 600* (d + step) ;
}
