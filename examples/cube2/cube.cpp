void main()
{
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);
    
    // Parameters for rotation
    float A = step * 0.02; // Rotation X
    float B = step * 0.02; // Rotation Y
    float C = step * 0.005; // Rotation Z
    
    // Cube size
    float cubeWidth = 45.0;
    int width = int(WSX);
    int height = int(WSY);
    
    // Convert pixel coordinates to normalized coordinates
    float nx = (float(x) / float(width) - 0.5) * 2.0;
    float ny = (float(y) / float(height) - 0.5) * 2.0;
    
    // Camera distance and scale factor
    float distanceFromCam = 90.0;
    float K1 = 70.0;
    
    // Drawing parameters
    float incrementSpeed = 5;
    float minDist = 999999.0;
    int color = 0xFF000000; // Default color (black)
    bool isOnCube = false; // Flag to track if the pixel is currently on the cube
    
    // For the first frame, initialize everything to black
    if (step == 0) {
        data_0[p] = 0xFF000000;
    } 
    else {
        // Fixed rotation to show 3 faces instead of using animated rotation
        // This ensures we always see the 3 faces properly
        float fixedA = 0.5; // Fixed angle to show 3 faces
        float fixedB = 0.3;
        float fixedC = 0.1;
        
        // Use a combination of fixed rotation and animated rotation
        float rotA = fixedA + A * 0.2;
        float rotB = fixedB + B * 0.2;
        float rotC = fixedC + C * 0.2;
        
        float threshold = 0.6;
        
        // Calculate faces with consistent dimensions to ensure proper cube shape
        for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
            for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed) {
                // Face 1: Right face (X = cubeWidth)
                {
                    float x1 = cubeWidth;
                    float y1 = cubeY;
                    float z1 = cubeX;
                    
                    // Apply rotation
                    float rotX1 = x1 * cos(rotB) * cos(rotC) + 
                                 z1 * sin(rotB) - 
                                 y1 * cos(rotB) * sin(rotC);
                    float rotY1 = x1 * (sin(rotA) * sin(rotB) * cos(rotC) + cos(rotA) * sin(rotC)) + 
                                 y1 * (cos(rotA) * cos(rotC) - sin(rotA) * sin(rotB) * sin(rotC)) - 
                                 z1 * sin(rotA) * cos(rotB);
                    float rotZ1 = x1 * (sin(rotA) * sin(rotC) - cos(rotA) * sin(rotB) * cos(rotC)) + 
                                 y1 * (cos(rotA) * sin(rotB) * sin(rotC) + sin(rotA) * cos(rotC)) + 
                                 z1 * cos(rotA) * cos(rotB);
                    
                    rotZ1 += distanceFromCam;
                    
                    if (rotZ1 > 0.0) {
                        float ooz1 = 1.0 / rotZ1;
                        float px1 = (width / 2.0) + K1 * ooz1 * rotX1;
                        float py1 = (height / 2.0) + K1 * ooz1 * rotY1;
                        
                        float dist1 = (px1 - float(x)) * (px1 - float(x)) + (py1 - float(y)) * (py1 - float(y));
                        if (dist1 < threshold && dist1 < minDist) {
                            minDist = dist1;
                            color = 0xFF0000FF; // Face 1: Blue
                            isOnCube = true;
                        }
                    }
                }
                
                // Face 2: Top face (Y = -cubeWidth)
                {
                    float x2 = cubeX;
                    float y2 = -cubeWidth;
                    float z2 = cubeY;
                    
                    // Apply rotation
                    float rotX2 = x2 * cos(rotB) * cos(rotC) + 
                                 z2 * sin(rotB) - 
                                 y2 * cos(rotB) * sin(rotC);
                    float rotY2 = x2 * (sin(rotA) * sin(rotB) * cos(rotC) + cos(rotA) * sin(rotC)) + 
                                 y2 * (cos(rotA) * cos(rotC) - sin(rotA) * sin(rotB) * sin(rotC)) - 
                                 z2 * sin(rotA) * cos(rotB);
                    float rotZ2 = x2 * (sin(rotA) * sin(rotC) - cos(rotA) * sin(rotB) * cos(rotC)) + 
                                 y2 * (cos(rotA) * sin(rotB) * sin(rotC) + sin(rotA) * cos(rotC)) + 
                                 z2 * cos(rotA) * cos(rotB);
                    
                    rotZ2 += distanceFromCam;
                    
                    if (rotZ2 > 0.0) {
                        float ooz2 = 1.0 / rotZ2;
                        float px2 = (width / 2.0) + K1 * ooz2 * rotX2;
                        float py2 = (height / 2.0) + K1 * ooz2 * rotY2;
                        
                        float dist2 = (px2 - float(x)) * (px2 - float(x)) + (py2 - float(y)) * (py2 - float(y));
                        if (dist2 < threshold && dist2 < minDist) {
                            minDist = dist2;
                            color = 0xFF00FF00; // Face 2: Green
                            isOnCube = true;
                        }
                    }
                }
                
                // Face 3: Front face (Z = cubeWidth)
                {
                    float x3 = cubeX;
                    float y3 = cubeY;
                    float z3 = cubeWidth;
                    
                    // Apply rotation
                    float rotX3 = x3 * cos(rotB) * cos(rotC) + 
                                 z3 * sin(rotB) - 
                                 y3 * cos(rotB) * sin(rotC);
                    float rotY3 = x3 * (sin(rotA) * sin(rotB) * cos(rotC) + cos(rotA) * sin(rotC)) + 
                                 y3 * (cos(rotA) * cos(rotC) - sin(rotA) * sin(rotB) * sin(rotC)) - 
                                 z3 * sin(rotA) * cos(rotB);
                    float rotZ3 = x3 * (sin(rotA) * sin(rotC) - cos(rotA) * sin(rotB) * cos(rotC)) + 
                                 y3 * (cos(rotA) * sin(rotB) * sin(rotC) + sin(rotA) * cos(rotC)) + 
                                 z3 * cos(rotA) * cos(rotB);
                    
                    rotZ3 += distanceFromCam;
                    
                    if (rotZ3 > 0.0) {
                        float ooz3 = 1.0 / rotZ3;
                        float px3 = (width / 2.0) + K1 * ooz3 * rotX3;
                        float py3 = (height / 2.0) + K1 * ooz3 * rotY3;
                        
                        float dist3 = (px3 - float(x)) * (px3 - float(x)) + (py3 - float(y)) * (py3 - float(y));
                        if (dist3 < threshold && dist3 < minDist) {
                            minDist = dist3;
                            color = 0xFFFF8000; // Face 3: Orange
                            isOnCube = true;
                        }
                    }
                }

                // Face 4: Right face (X = cubeWidth)
                {
                    float x1 = -cubeWidth;
                    float y1 = cubeY;
                    float z1 = cubeX;
                    
                    // Apply rotation
                    float rotX1 = x1 * cos(rotB) * cos(rotC) + 
                                 z1 * sin(rotB) - 
                                 y1 * cos(rotB) * sin(rotC);
                    float rotY1 = x1 * (sin(rotA) * sin(rotB) * cos(rotC) + cos(rotA) * sin(rotC)) + 
                                 y1 * (cos(rotA) * cos(rotC) - sin(rotA) * sin(rotB) * sin(rotC)) - 
                                 z1 * sin(rotA) * cos(rotB);
                    float rotZ1 = x1 * (sin(rotA) * sin(rotC) - cos(rotA) * sin(rotB) * cos(rotC)) + 
                                 y1 * (cos(rotA) * sin(rotB) * sin(rotC) + sin(rotA) * cos(rotC)) + 
                                 z1 * cos(rotA) * cos(rotB);
                    
                    rotZ1 += distanceFromCam;
                    
                    if (rotZ1 > 0.0) {
                        float ooz1 = 1.0 / rotZ1;
                        float px1 = (width / 2.0) + K1 * ooz1 * rotX1;
                        float py1 = (height / 2.0) + K1 * ooz1 * rotY1;
                        
                        float dist1 = (px1 - float(x)) * (px1 - float(x)) + (py1 - float(y)) * (py1 - float(y));
                        if (dist1 < threshold && dist1 < minDist) {
                            minDist = dist1;
                            color = 0xFF0000FF; // Face 1: Blue
                            isOnCube = true;
                        }
                    }
                }

                // Face 5: Top face (Y = -cubeWidth)
                {
                    float x2 = cubeX;
                    float y2 = cubeWidth;
                    float z2 = cubeY;
                    
                    // Apply rotation
                    float rotX2 = x2 * cos(rotB) * cos(rotC) + 
                                 z2 * sin(rotB) - 
                                 y2 * cos(rotB) * sin(rotC);
                    float rotY2 = x2 * (sin(rotA) * sin(rotB) * cos(rotC) + cos(rotA) * sin(rotC)) + 
                                 y2 * (cos(rotA) * cos(rotC) - sin(rotA) * sin(rotB) * sin(rotC)) - 
                                 z2 * sin(rotA) * cos(rotB);
                    float rotZ2 = x2 * (sin(rotA) * sin(rotC) - cos(rotA) * sin(rotB) * cos(rotC)) + 
                                 y2 * (cos(rotA) * sin(rotB) * sin(rotC) + sin(rotA) * cos(rotC)) + 
                                 z2 * cos(rotA) * cos(rotB);
                    
                    rotZ2 += distanceFromCam;
                    
                    if (rotZ2 > 0.0) {
                        float ooz2 = 1.0 / rotZ2;
                        float px2 = (width / 2.0) + K1 * ooz2 * rotX2;
                        float py2 = (height / 2.0) + K1 * ooz2 * rotY2;
                        
                        float dist2 = (px2 - float(x)) * (px2 - float(x)) + (py2 - float(y)) * (py2 - float(y));
                        if (dist2 < threshold && dist2 < minDist) {
                            minDist = dist2;
                            color = 0xFF00FF00; // Face 2: Green
                            isOnCube = true;
                        }
                    }
                }

                // Face 6: Front face (Z = cubeWidth)
                {
                    float x3 = cubeX;
                    float y3 = cubeY;
                    float z3 = -cubeWidth;
                    
                    // Apply rotation
                    float rotX3 = x3 * cos(rotB) * cos(rotC) + 
                                 z3 * sin(rotB) - 
                                 y3 * cos(rotB) * sin(rotC);
                    float rotY3 = x3 * (sin(rotA) * sin(rotB) * cos(rotC) + cos(rotA) * sin(rotC)) + 
                                 y3 * (cos(rotA) * cos(rotC) - sin(rotA) * sin(rotB) * sin(rotC)) - 
                                 z3 * sin(rotA) * cos(rotB);
                    float rotZ3 = x3 * (sin(rotA) * sin(rotC) - cos(rotA) * sin(rotB) * cos(rotC)) + 
                                 y3 * (cos(rotA) * sin(rotB) * sin(rotC) + sin(rotA) * cos(rotC)) + 
                                 z3 * cos(rotA) * cos(rotB);
                    
                    rotZ3 += distanceFromCam;
                    
                    if (rotZ3 > 0.0) {
                        float ooz3 = 1.0 / rotZ3;
                        float px3 = (width / 2.0) + K1 * ooz3 * rotX3;
                        float py3 = (height / 2.0) + K1 * ooz3 * rotY3;
                        
                        float dist3 = (px3 - float(x)) * (px3 - float(x)) + (py3 - float(y)) * (py3 - float(y));
                        if (dist3 < threshold && dist3 < minDist) {
                            minDist = dist3;
                            color = 0xFFFF8000; // Face 3: Orange
                            isOnCube = true;
                        }
                    }
                }
            }
        }
        
        // No trailing effect
        if (isOnCube) {
            data_0[p] = color;
        } else {
            data_0[p] = 0xFF000000;
        }
    }
}

