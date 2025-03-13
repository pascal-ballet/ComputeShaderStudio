void main()
{
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);
    
    // Paramètres généraux
    int width = int(WSX);
    int height = int(WSY);
    float distanceFromCam = 150.0;
    float K1 = 70.0;
    
    // Pour le premier frame, initialiser tout en noir
    if (step == 0) {
        data_0[p] = 0xFF260B42;
    } 
    else {
        // Nombre de cubes
        const int numCubes = 3;
        
        float minDist = 999999.0;
        int color = 0xFF260B42; // Couleur par défaut (noir)
        bool isOnCube = false; // Indicateur si le pixel est sur un cube
        
        // Paramètres individuels pour chaque cube
        float cubeWidth[numCubes] = float[](35.0, 35.0, 35.0);
        
        // Espacer les cubes horizontalement et ajouter mouvement aléatoire
        // Utiliser différentes fonctions de bruit pseudo-aléatoire basées sur le temps
        vec3 cubePositions[numCubes] = vec3[](
            // Cube 1: position de base à gauche + mouvement aléatoire
            vec3(-100.0 + sin(step * 0.023) * 20.0 + cos(step * 0.019) * 15.0, 
                 sin(step * 0.017) * 25.0 + cos(step * 0.011) * 10.0, 
                 sin(step * 0.013) * 15.0 + cos(step * 0.027) * 20.0),
            
            // Cube 2: position de base au centre + mouvement aléatoire
            vec3(0.0 + sin(step * 0.021) * 15.0 + cos(step * 0.015) * 25.0, 
                 sin(step * 0.012) * 20.0 + cos(step * 0.025) * 15.0, 
                 sin(step * 0.018) * 25.0 + cos(step * 0.022) * 10.0),
            
            // Cube 3: position de base à droite + mouvement aléatoire
            vec3(100.0 + sin(step * 0.016) * 25.0 + cos(step * 0.024) * 15.0, 
                 sin(step * 0.026) * 15.0 + cos(step * 0.014) * 20.0, 
                 sin(step * 0.022) * 20.0 + cos(step * 0.018) * 25.0)
        );
        
        // Rotations différentes pour chaque cube
        vec3 cubeRotations[numCubes] = vec3[](
            vec3(step * 0.017, step * 0.019, step * 0.013),
            vec3(step * 0.015, step * 0.021, step * 0.018),
            vec3(step * 0.012, step * 0.023, step * 0.016)
        );
        
        int cubeColors[numCubes][3] = int[3][](
            int[](0xFF0000FF, 0xFF00FF00, 0xFFFF8000), // Cube 1: Bleu, Vert, Orange
            int[](0xFFFF0000, 0xFF00FFFF, 0xFFFF00FF), // Cube 2: Rouge, Cyan, Magenta
            int[](0xFFFFFF00, 0xFF8000FF, 0xFF00FF80)  // Cube 3: Jaune, Violet, Vert clair
        );
        
        // Paramètres de dessin
        float incrementSpeed = 5.0;
        float threshold = 0.1; // Points encore plus gros
        
        // Traiter chaque cube
        for (int cubeIndex = 0; cubeIndex < numCubes; cubeIndex++) {
            // Rotation fixe + animée
            float fixedA = 0.5; // Angle fixe pour montrer 3 faces
            float fixedB = 0.3;
            float fixedC = 0.1;
            
            vec3 rot = cubeRotations[cubeIndex];
            float rotA = fixedA + rot.x * 0.2;
            float rotB = fixedB + rot.y * 0.2;
            float rotC = fixedC + rot.z * 0.2;
            
            // Position du cube
            vec3 cubePos = cubePositions[cubeIndex];
            float currentCubeWidth = cubeWidth[cubeIndex];
            
            // Calculer les faces avec des dimensions cohérentes
            for (float cubeX = -currentCubeWidth; cubeX < currentCubeWidth; cubeX += incrementSpeed) {
                for (float cubeY = -currentCubeWidth; cubeY < currentCubeWidth; cubeY += incrementSpeed) {
                    // Face 1: Face droite (X = currentCubeWidth)
                    {
                        float x1 = currentCubeWidth;
                        float y1 = cubeY;
                        float z1 = cubeX;
                        
                        // Ajouter le décalage de position
                        x1 += cubePos.x;
                        y1 += cubePos.y;
                        z1 += cubePos.z;
                        
                        // Appliquer la rotation
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
                    
                    // Face 2: Face supérieure (Y = -currentCubeWidth)
                    {
                        float x2 = cubeX;
                        float y2 = -currentCubeWidth;
                        float z2 = cubeY;
                        
                        // Ajouter le décalage de position
                        x2 += cubePos.x;
                        y2 += cubePos.y;
                        z2 += cubePos.z;
                        
                        // Appliquer la rotation
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
                                color = cubeColors[cubeIndex][1]; // Face 2 color
                                isOnCube = true;
                            }
                        }
                    }
                    
                    // Face 3: Face avant (Z = currentCubeWidth)
                    {
                        float x3 = cubeX;
                        float y3 = cubeY;
                        float z3 = currentCubeWidth;
                        
                        // Ajouter le décalage de position
                        x3 += cubePos.x;
                        y3 += cubePos.y;
                        z3 += cubePos.z;
                        
                        // Appliquer la rotation
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
                                color = cubeColors[cubeIndex][2]; // Face 3 color
                                isOnCube = true;
                            }
                        }
                    }
                    // Face 1: Face droite (X = currentCubeWidth)
                    {
                        float x1 = -currentCubeWidth;
                        float y1 = cubeY;
                        float z1 = cubeX;
                        
                        // Ajouter le décalage de position
                        x1 += cubePos.x;
                        y1 += cubePos.y;
                        z1 += cubePos.z;
                        
                        // Appliquer la rotation
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
                    
                    // Face 2: Face supérieure (Y = -currentCubeWidth)
                    {
                        float x2 = cubeX;
                        float y2 = currentCubeWidth;
                        float z2 = cubeY;
                        
                        // Ajouter le décalage de position
                        x2 += cubePos.x;
                        y2 += cubePos.y;
                        z2 += cubePos.z;
                        
                        // Appliquer la rotation
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
                                color = cubeColors[cubeIndex][1]; // Face 2 color
                                isOnCube = true;
                            }
                        }
                    }
                    
                    // Face 3: Face avant (Z = currentCubeWidth)
                    {
                        float x3 = cubeX;
                        float y3 = cubeY;
                        float z3 = -currentCubeWidth;
                        
                        // Ajouter le décalage de position
                        x3 += cubePos.x;
                        y3 += cubePos.y;
                        z3 += cubePos.z;
                        
                        // Appliquer la rotation
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
                                color = cubeColors[cubeIndex][2]; // Face 3 color
                                isOnCube = true;
                            }
                        }
                    }
                }
            }
        }
        
        // Pas d'effet de traînée
        if (isOnCube) {
            data_0[p] = color;
        } else {
            data_0[p] = 0xFF641E16; // Noir pour les pixels qui ne sont pas sur un cube
        }
    }
}