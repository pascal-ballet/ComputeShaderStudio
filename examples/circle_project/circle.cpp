void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    float centerX = WSX / 2.0;
    float centerY = WSY / 2.0;
    float radius = WSX / 4.0;

    float dx = float(x) - centerX;
    float dy = float(y) - centerY;

    float angle = step * 0.05;
    float cosA = cos(angle);
    float sinA = sin(angle);

    float rotatedX = cosA * dx - sinA * dy;
    float rotatedY = sinA * dx + cosA * dy;

    float dist = sqrt(rotatedX * rotatedX + rotatedY * rotatedY);

    float intensity = exp(-pow((dist - radius) / 10.0, 2.0));
    float twistFactor = sin(dist * 0.1 + step * 0.02);

    int red = int(255.0 * intensity);
    int green = int(150.0 * intensity * twistFactor);
    int blue = int(255.0 * (1.0 - intensity));
    int alpha = 255;

    data_0[p] = (alpha << 24) | (red << 16) | (green << 8) | blue;
}
