

void main() {
    float angle = step * 0.01;  

    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    int cx = int(WSX) / 2;  
    int cy = int(WSY) / 2;  

    int dx = int(x) - cx;
    int dy = int(y) - cy;

    int trait1 = 450;  

    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    int planetX = int(cx + trait1 * cosAngle);
    int planetY = int(cy + trait1 * sinAngle);  

    int distTerre = (int(x) - cx) * (int(x) - cx) + (int(y) - cy) * (int(y) - cy);
    int Terre = 250 * 250; 

    int distTrait1 = (int(x) - cx) * (int(x) - cx) + (int(y) - cy) * (int(y) - cy);

    int distPlanete = (int(x) - planetX) * (int(x) - planetX) + (int(y) - planetY) * (int(y) - planetY);
    int planetRadius = 50 * 50;  

	int randomSeed = int(x * 37 + y * 89); 
    int noise = (randomSeed % 100) - 50;

	if (distTerre <= Terre) {
        data_0[p] = 0xFF0000FF; 
    }
    else if (distPlanete <= planetRadius) {
        data_0[p] = 0xFFFFFF00; 
    }
	else if (distTrait1 <= (trait1 * trait1) + 1000 && distTrait1 >= (trait1 * trait1) - 1000) {
        data_0[p] = 0xFF808080;
    }
    else {
            data_0[p] = 0xFF000000; // Fond noir (R = 0, G = 0, B = 0, A = 255)
    }
    
}
