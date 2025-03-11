void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
    // Circle parameters
    float centerX = WSX / 2.0;
    float centerY = WSY / 2.0;
    float radius = 70.0;
    
    // Angle de rotation
    float angle = float(step) * 0.1;
    
    // Calculate distance from current pixel to circle center
    float dx = float(x) - centerX;
    float dy = float(y) - centerY;
    float distance = sqrt(dx * dx + dy * dy);
    
    // Calculate angle of current pixel relative to center
    float pixelAngle = atan(dy, dx);
    
    // Width of the line in radians
    float lineWidth = 0.1;
    
    // Check if pixel is on the rotating line
    bool onLine = abs(mod(pixelAngle - angle, 2.0 * 3.14159)) < lineWidth;
    
    // Inner circle parameters with pulsing effect
    float baseInnerRadius = 18.0;
    float pulseAmplitude = 3.0; // Change in radius (20 to 23)
    float pulseFrequency = 1.5; // Réduit de 3.0 à 1.5 pour un pulsage plus lent
    float innerRadius = baseInnerRadius + pulseAmplitude; //* sin(float(step) * pulseFrequency * 0.1);
    
    // Border circle parameters
    float borderWidth = 2.0;
    float borderRadius = innerRadius + 15.0; // Un peu plus grand que le cercle intérieur
    
    // Moving circle parameters
    float movingCircleRadius = 5.0;
    float movingCircleX = centerX + borderRadius * cos(angle);
    float movingCircleY = centerY + borderRadius * sin(angle);
    float movingCircleDistance = sqrt(pow(float(x) - movingCircleX, 2.0) + pow(float(y) - movingCircleY, 2.0));

    // Draw circle with line and inner circle
    if (distance < radius) {
        if (movingCircleDistance < movingCircleRadius) {
            data_0[p] = 0xFF000000; // Black moving circle
        } else if (distance < innerRadius) {
            data_0[p] = 0xFF000000; // Black inner circle
        } else if (distance < borderRadius && distance > borderRadius - borderWidth) {
            data_0[p] = 0xFF000000; // Black border
        } else if (onLine) {
            //data_0[p] = 0xFF000000; // Black line
        } else {
            data_0[p] = 0xFF0000FF; // Red circle
        }
    } else {
        data_0[p] = 0xFFFFFFFF; // White background
    }
}
