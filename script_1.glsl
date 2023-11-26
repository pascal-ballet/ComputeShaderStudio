#[compute]
#version 450

// Invocations in the (x, y, z) dimension
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// A binding to the buffer we create in our script
layout(binding = 0) buffer Data1 {
    int data_1[];
};

layout(binding = 1) buffer Data2 {
    int data_2[];
};

layout(binding = 2) buffer Params {
    int stage;
};

// The code we want to execute in each invocation
void main() {
	data_1[gl_GlobalInvocationID.x] ++;
	
	//if (stage == 0)
    //    data_1[gl_GlobalInvocationID.x] += 10;
	//if (stage == 1)
    //    data_1[gl_GlobalInvocationID.x] -= 10;
        
}
