// Write your code HERE
void main() {
	float angle = step * 0.01; 

	float angle1 = step * 0.01; 

	float angle2 = step * 0.01; 

	float angle3 = step * 0.01; 


    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    int cx = int(WSX) / 2;
    int cy = int(WSY) / 2;

    int dx = int(x) - cx;
    int dy = int(y) - cy;

    // Calcul de l'angle de rotation (par exemple, utiliser le temps ou un compteur pour l'animation)

    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    int newDx = int(dx * cosAngle - dy * sinAngle);
    int newDy = int(dx * sinAngle + dy * cosAngle);

    int newX = newDx + cx;
    int newY = newDy + cy;

    // Calcul de la distance entre ce pixel et le centre de l'image
    int dist = (newX - cx) * (newX - cx) + (newY - cy) * (newY - cy);

    // Rayon du cercle (150 pixels de rayon)
    int radius = 250 * 250;

	int trait1 = 450 * 450;

	int planetRadius = 50;
	int planetX = int(cx + 450 * cosAngle); 
    int planetY = int(cy + 450 * sinAngle);

	int dist2 = (x - planetX) * (x - planetX) + (y - planetY) * (y - planetY);

	int radiusPlanet = planetRadius * planetRadius;

    int dist2 = (x - cx) * (x - cx) + (y - cy) * (y - cy);
    int radiusEarth = 250 * 250; // Rayon de la Terre

    if (dist2 <= radiusEarth) {
        data_0[p] = 0xFF0000FF;
    }
    else if (dist <= radiusPlanet) {
        data_0[p] = 0xFFFFFF00; 
    }
    else {
        data_0[p] = 0xFF000000; // Fond noir (R = 0, G = 0, B = 0, A = 255)
    }
}