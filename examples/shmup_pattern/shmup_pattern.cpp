#define RED 0xFFFF0000
#define GREEN 0xFF00FF00
#define BLUE 0xFF0000FF
#define BLACK 0xFF000000


void render(uint x, uint y, uint p)
{
	if((x==15) && (y==16))
		data_0[p] = GREEN;
	else
		data_0[p] = BLACK;
}

void main()
{
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint p = x + y * WSX;


	render(x,y,p);
}