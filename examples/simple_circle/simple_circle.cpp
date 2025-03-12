void main()
{
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);
    
    // Parameters for rotation
    float A = step * 0.05; // Rotation X
    float B = step * 0.05; // Rotation Y
    float C = step * 0.01; // Rotation Z
    
    float cubeWidth = 20.0;
    int width = int(WSX);
    int height = int(WSY);
    
    // Convert pixel coordinates to normalized coordinates
    float nx = (float(x) / float(width) - 0.5) * 2.0;
    float ny = (float(y) / float(height) - 0.5) * 2.0;
    
    // Camera distance and scale factor
    float distanceFromCam = 100.0;
    float K1 = 40.0;
    
    // Drawing parameters
    float incrementSpeed = 0.6;
    float minDist = 999999.0;
    int color = 0xFF000000; // Default color (black)
    
    // Initialize background on first step
    if (step == 0) {
        data_0[p] = 0xFF000000;
    } 
    else {
        // For each face of the cube, check if the current pixel is on this face
        for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
            for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed) {
                // Calculate 3D coordinates for different faces of the cube
                // Face 1: Z = -cubeWidth
                float x1 = cubeX * sin(A) * sin(B) * cos(C) - (-cubeWidth) * cos(A) * sin(B) * cos(C) +
                        cubeX * cos(A) * sin(C) + (-cubeWidth) * sin(A) * sin(C) + cubeY * cos(B) * cos(C);
                float y1 = cubeX * cos(A) * cos(C) + (-cubeWidth) * sin(A) * cos(C) -
                        cubeX * sin(A) * sin(B) * sin(C) + (-cubeWidth) * cos(A) * sin(B) * sin(C) -
                        cubeY * cos(B) * sin(C);
                float z1 = (-cubeWidth) * cos(A) * cos(B) - cubeX * sin(A) * cos(B) + cubeY * sin(B) + distanceFromCam;
                
                if (z1 > 0.0) {
                    float ooz1 = 1.0 / z1;
                    float px1 = (width / 2.0) + K1 * ooz1 * x1 * 2.0;
                    float py1 = (height / 2.0) + K1 * ooz1 * y1;
                    
                    float dist1 = (px1 - float(x)) * (px1 - float(x)) + (py1 - float(y)) * (py1 - float(y));
                    if (dist1 < 1.0 && dist1 < minDist) {
                        minDist = dist1;
                        color = 0xFF0000FF; // Face 1: Red
                    }
                }
                
                // Face 2: X = cubeWidth
                float x2 = cubeY * sin(A) * sin(B) * cos(C) - cubeWidth * cos(A) * sin(B) * cos(C) +
                        cubeY * cos(A) * sin(C) + cubeWidth * sin(A) * sin(C) + cubeWidth * cos(B) * cos(C);
                float y2 = cubeY * cos(A) * cos(C) + cubeWidth * sin(A) * cos(C) -
                        cubeY * sin(A) * sin(B) * sin(C) + cubeWidth * cos(A) * sin(B) * sin(C) -
                        cubeWidth * cos(B) * sin(C);
                float z2 = cubeWidth * cos(A) * cos(B) - cubeY * sin(A) * cos(B) + cubeWidth * sin(B) + distanceFromCam;
                
                if (z2 > 0.0) {
                    float ooz2 = 1.0 / z2;
                    float px2 = (width / 2.0) + K1 * ooz2 * x2 * 2.0;
                    float py2 = (height / 2.0) + K1 * ooz2 * y2;
                    
                    float dist2 = (px2 - float(x)) * (px2 - float(x)) + (py2 - float(y)) * (py2 - float(y));
                    if (dist2 < 1.0 && dist2 < minDist) {
                        minDist = dist2;
                        color = 0xFF00FF00; // Face 2: Green
                    }
                }
                
                // Face 3: X = -cubeWidth
                float x3 = cubeY * sin(A) * sin(B) * cos(C) - (-cubeWidth) * cos(A) * sin(B) * cos(C) +
                        cubeY * cos(A) * sin(C) + (-cubeWidth) * sin(A) * sin(C) + (-cubeWidth) * cos(B) * cos(C);
                float y3 = cubeY * cos(A) * cos(C) + (-cubeWidth) * sin(A) * cos(C) -
                        cubeY * sin(A) * sin(B) * sin(C) + (-cubeWidth) * cos(A) * sin(B) * sin(C) -
                        (-cubeWidth) * cos(B) * sin(C);
                float z3 = (-cubeWidth) * cos(A) * cos(B) - cubeY * sin(A) * cos(B) + (-cubeWidth) * sin(B) + distanceFromCam;
                
                if (z3 > 0.0) {
                    float ooz3 = 1.0 / z3;
                    float px3 = (width / 2.0) + K1 * ooz3 * x3 * 2.0;
                    float py3 = (height / 2.0) + K1 * ooz3 * y3;
                    
                    float dist3 = (px3 - float(x)) * (px3 - float(x)) + (py3 - float(y)) * (py3 - float(y));
                    if (dist3 < 1.0 && dist3 < minDist) {
                        minDist = dist3;
                        color = 0xFF0080FF; // Face 3: Light blue
                    }
                }
                
                // Face 4: Z = cubeWidth
                float x4 = cubeX * sin(A) * sin(B) * cos(C) - cubeWidth * cos(A) * sin(B) * cos(C) +
                        cubeX * cos(A) * sin(C) + cubeWidth * sin(A) * sin(C) + cubeY * cos(B) * cos(C);
                float y4 = cubeX * cos(A) * cos(C) + cubeWidth * sin(A) * cos(C) -
                        cubeX * sin(A) * sin(B) * sin(C) + cubeWidth * cos(A) * sin(B) * sin(C) -
                        cubeY * cos(B) * sin(C);
                float z4 = cubeWidth * cos(A) * cos(B) - cubeX * sin(A) * cos(B) + cubeY * sin(B) + distanceFromCam;
                
                if (z4 > 0.0) {
                    float ooz4 = 1.0 / z4;
                    float px4 = (width / 2.0) + K1 * ooz4 * x4 * 2.0;
                    float py4 = (height / 2.0) + K1 * ooz4 * y4;
                    
                    float dist4 = (px4 - float(x)) * (px4 - float(x)) + (py4 - float(y)) * (py4 - float(y));
                    if (dist4 < 1.0 && dist4 < minDist) {
                        minDist = dist4;
                        color = 0xFFFF00FF; // Face 4: Magenta
                    }
                }
                
                // Face 5: Y = -cubeWidth
                float x5 = cubeX * sin(A) * sin(B) * cos(C) - cubeY * cos(A) * sin(B) * cos(C) +
                        cubeX * cos(A) * sin(C) + cubeY * sin(A) * sin(C) + (-cubeWidth) * cos(B) * cos(C);
                float y5 = cubeX * cos(A) * cos(C) + cubeY * sin(A) * cos(C) -
                        cubeX * sin(A) * sin(B) * sin(C) + cubeY * cos(A) * sin(B) * sin(C) -
                        (-cubeWidth) * cos(B) * sin(C);
                float z5 = cubeY * cos(A) * cos(B) - cubeX * sin(A) * cos(B) + (-cubeWidth) * sin(B) + distanceFromCam;
                
                if (z5 > 0.0) {
                    float ooz5 = 1.0 / z5;
                    float px5 = (width / 2.0) + K1 * ooz5 * x5 * 2.0;
                    float py5 = (height / 2.0) + K1 * ooz5 * y5;
                    
                    float dist5 = (px5 - float(x)) * (px5 - float(x)) + (py5 - float(y)) * (py5 - float(y));
                    if (dist5 < 1.0 && dist5 < minDist) {
                        minDist = dist5;
                        color = 0xFFFFFF00; // Face 5: Yellow
                    }
                }
                
                // Face 6: Y = cubeWidth
                float x6 = cubeX * sin(A) * sin(B) * cos(C) - cubeY * cos(A) * sin(B) * cos(C) +
                        cubeX * cos(A) * sin(C) + cubeY * sin(A) * sin(C) + cubeWidth * cos(B) * cos(C);
                float y6 = cubeX * cos(A) * cos(C) + cubeY * sin(A) * cos(C) -
                        cubeX * sin(A) * sin(B) * sin(C) + cubeY * cos(A) * sin(B) * sin(C) -
                        cubeWidth * cos(B) * sin(C);
                float z6 = cubeY * cos(A) * cos(B) - cubeX * sin(A) * cos(B) + cubeWidth * sin(B) + distanceFromCam;
                
                if (z6 > 0.0) {
                    float ooz6 = 1.0 / z6;
                    float px6 = (width / 2.0) + K1 * ooz6 * x6 * 2.0;
                    float py6 = (height / 2.0) + K1 * ooz6 * y6;
                    
                    float dist6 = (px6 - float(x)) * (px6 - float(x)) + (py6 - float(y)) * (py6 - float(y));
                    if (dist6 < 1.0 && dist6 < minDist) {
                        minDist = dist6;
                        color = 0xFF00FFFF; // Face 6: Cyan
                    }
                }
            }
        }
        
        // Update the pixel color if we found a close face
        if (minDist < 999999.0) {
            data_0[p] = color;
        } else {
            // Gradually darken the background
            int pix = data_0[p];
            int col = pix & 0x00FFFFFF;
            if (col > 0) {
                data_0[p] = pix - 1;
            }
        }
    }
}