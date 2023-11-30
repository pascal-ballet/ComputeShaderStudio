
#[compute]
#version 450

// Invocations in the (x, y, z) dimension
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Bindings to the buffers we create in our script
layout(binding = 0) buffer Params {
	int step;
	int current_pass;
};


uint WSX=128;
uint WSY=128;

layout(binding = 1) buffer Data0 {
	int data_0[];
};


layout(binding = 2) buffer Data1 {
	int data_1[];
};


// Write your code HERE
void main() {
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint p = x + y * WSX;
	data_0[p] = 0xFFF00FFF - int(p)*(step+1);
	data_1[p] = 0xFF00AA00 + int( 1.0 + 99999.9*sin(float(x+float(step+y))/1000.0));
}
