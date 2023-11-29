
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
	
	if (step == 1 && current_pass == 0) {
		if (data_0[p] < 999990){
			data_0[p] = 0x0000FF00;
		} else {
			data_0[p] = 0x00FF0000;
		}
	}
	
	if (current_pass == 0) {
		//data_0[p] = data_0[p] / 2;
		data_1[p] = data_1[p] + 1024;
	}
	
}
