void main()
{
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);
    
    // General parameters
    int width = int(WSX);
    int height = int(WSY);
    float distanceFromCam = 150.0;
    float K1 = 70.0;
    
    // Background color constant
    int backgroundColor = 0xFF641E16;
    
    // For the first frame, initialize everything with background color
    if (step == 0) {
        data_0[p] = backgroundColor;
    } 
    else {
        // Number of cubes
        const int numCubes = 3;
        
        float minDist = 999999.0;
        int color = backgroundColor; // Default color (background)
        bool isOnCube = false; // Indicator if pixel is on a cube
        
        // Individual parameters for each cube
        float cubeWidth[numCubes] = float[](35.0, 35.0, 35.0);

        // Space cubes horizontally and add more lateral movement
        vec3 cubePositions[numCubes] = vec3[](
            // Cube 1: rotated 90 up
            vec3(-100.0 + sin(step * 0.010) * 50.0, 
                sin(step * 0.006) * 15.0, 
                cos(step * 0.015) * 40.0),

            // Cube 2: rotated 90 up
            vec3(0.0 + sin(step * 0.011) * 60.0, 
                sin(step * 0.009) * 20.0, 
                cos(step * 0.007) * 45.0),

            // Cube 3: rotated 90 up
            vec3(100.0 + sin(step * 0.008) * 55.0, 
                sin(step * 0.011) * 25.0, 
                cos(step * 0.014) * 35.0)
        );


        
        // Different rotations for each cube - reduced for smoother movement
        vec3 cubeRotations[numCubes] = vec3[](
            vec3(step * 0.008, step * 0.009, step * 0.007),
            vec3(step * 0.007, step * 0.010, step * 0.009),
            vec3(step * 0.006, step * 0.011, step * 0.008)
        );
        
        int cubeColors[numCubes][6] = int[3][](
            int[](0xFF0000FF, 0xFF00FF00, 0xFFFF8000, 0xFF0080FF, 0xFFFF00FF, 0xFF80FF00), // Cube 1
            int[](0xFFFF0000, 0xFF00FFFF, 0xFFFF00FF, 0xFF8000FF, 0xFF00FF80, 0xFFFFFF00), // Cube 2
            int[](0xFFFFFF00, 0xFF8000FF, 0xFF00FF80, 0xFFFF0000, 0xFF00FFFF, 0xFFFF8000)  // Cube 3
        );
        
        // IMPROVED drawing parameters
        float incrementSpeed = 5.0; // Much smaller for better density
        float threshold = 0.3;      // Larger for slightly overlapping points
        
        // Process each cube
        for (int cubeIndex = 0; cubeIndex < numCubes; cubeIndex++) {
            // Fixed + animated rotation
            float fixedA = 0.5; // Fixed angle to show 3 faces
            float fixedB = 0.3;
            float fixedC = 0.1;
            
            vec3 rot = cubeRotations[cubeIndex];
            float rotA = fixedA + rot.x * 0.2;
            float rotB = fixedB + rot.y * 0.2;
            float rotC = fixedC + rot.z * 0.2;
            
            // Cube position
            vec3 cubePos = cubePositions[cubeIndex];
            float currentCubeWidth = cubeWidth[cubeIndex];
            
            // Calculate all 6 cube faces for complete representation
            for (float cubeX = -currentCubeWidth; cubeX <= currentCubeWidth; cubeX += incrementSpeed) {
                for (float cubeY = -currentCubeWidth; cubeY <= currentCubeWidth; cubeY += incrementSpeed) {
                    // Face 1: Right face (X = currentCubeWidth)
                    {
                        float x1 = currentCubeWidth;
                        float y1 = cubeY;
                        float z1 = cubeX;
                        
                        x1 += cubePos.x;
                        y1 += cubePos.y;
                        z1 += cubePos.z;
                        
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
                                color = cubeColors[cubeIndex][0]; // Face 1 color
                                isOnCube = true;
                            }
                        }
                    }
                    
                    // Face 2: Left face (X = -currentCubeWidth)
                    {
                        float x1 = -currentCubeWidth;
                        float y1 = cubeY;
                        float z1 = cubeX;
                        
                        x1 += cubePos.x;
                        y1 += cubePos.y;
                        z1 += cubePos.z;
                        
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
                                color = cubeColors[cubeIndex][1]; // Face 2 color
                                isOnCube = true;
                            }
                        }
                    }
                    
                    // Face 3: Top face (Y = -currentCubeWidth)
                    {
                        float x2 = cubeX;
                        float y2 = -currentCubeWidth;
                        float z2 = cubeY;
                        
                        x2 += cubePos.x;
                        y2 += cubePos.y;
                        z2 += cubePos.z;
                        
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
                                color = cubeColors[cubeIndex][2]; // Face 3 color
                                isOnCube = true;
                            }
                        }
                    }
                    
                    // Face 4: Bottom face (Y = currentCubeWidth)
                    {
                        float x2 = cubeX;
                        float y2 = currentCubeWidth;
                        float z2 = cubeY;
                        
                        x2 += cubePos.x;
                        y2 += cubePos.y;
                        z2 += cubePos.z;
                        
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
                                color = cubeColors[cubeIndex][3]; // Face 4 color
                                isOnCube = true;
                            }
                        }
                    }
                    
                    // Face 5: Front face (Z = currentCubeWidth)
                    {
                        float x3 = cubeX;
                        float y3 = cubeY;
                        float z3 = currentCubeWidth;
                        
                        x3 += cubePos.x;
                        y3 += cubePos.y;
                        z3 += cubePos.z;
                        
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
                                color = cubeColors[cubeIndex][4]; // Face 5 color
                                isOnCube = true;
                            }
                        }
                    }
                    
                    // Face 6: Back face (Z = -currentCubeWidth)
                    {
                        float x3 = cubeX;
                        float y3 = cubeY;
                        float z3 = -currentCubeWidth;
                        
                        x3 += cubePos.x;
                        y3 += cubePos.y;
                        z3 += cubePos.z;
                        
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
                                color = cubeColors[cubeIndex][5]; // Face 6 color
                                isOnCube = true;
                            }
                        }
                    }
                }
            }
        }
        
        // Apply color to current pixel
        data_0[p] = isOnCube ? color : backgroundColor;
    }
}