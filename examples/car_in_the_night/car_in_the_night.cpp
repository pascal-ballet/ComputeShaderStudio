#define CAR_WIDTH 60.0
#define CAR_HEIGHT 30.0
#define WHEEL_RADIUS 8.0
#define SPEED 2.0
#define RAIN_DROPS 500
#define SPLASH_PARTICLES 20
#define CAR_COUNT 8  // Plus de voitures pour remplir les 4 voies
#define STAR_COUNT 200  // Nombre d'étoiles dans le ciel

float hash(float n) { return fract(sin(n) * 43758.5453123); }

float noise(vec2 p) { 
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f*f*(3.0-2.0*f);
    return mix(
        mix(hash(i.x + i.y*157.0), hash(i.x+1.0 + i.y*157.0), f.x),
        mix(hash(i.x + (i.y+1.0)*157.0), hash(i.x+1.0 + (i.y+1.0)*157.0), f.x), 
        f.y
    );
}

// Fonction pour créer un bruit fractal (FBM - Fractional Brownian Motion)
float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    // Additionner plusieurs octaves de bruit
    for (int i = 0; i < 5; i++) {
        value += amplitude * noise(p * frequency);
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    return value;
}

void main() {
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);
    
    // Définir les limites des différentes zones
    float skyLimit = float(WSX) / 4.0 - 10.0;  // Limite entre ciel et route
    float laneY1 = float(WSX) / 4.0;         // Voie du haut (voitures vers la droite - lente)
    float laneY4 = float(WSX) / 4.0 + 115;    // Voie du bas (voitures vers la gauche - lente)
    float grassStart = laneY4 + 30.0;         // Début de l'herbe après la route
    
    // Ciel nocturne avec étoiles
    if (y < int(skyLimit)) {
        // Dégradé pour le ciel nocturne (plus foncé en haut, plus clair vers l'horizon)
        float skyGradient = float(y) / skyLimit;
        vec3 skyColor = mix(
            vec3(0.02, 0.03, 0.15),  // Bleu nuit très foncé (haut)
            vec3(0.05, 0.07, 0.2),   // Bleu nuit un peu plus clair (vers l'horizon)
            skyGradient
        );
        
        // Appliquer la couleur de base du ciel
        data_0[p] = 0xFF000000 | (int(skyColor.r * 255.0) << 16) | (int(skyColor.g * 255.0) << 8) | int(skyColor.b * 255.0);
        
        // Ajouter des étoiles
        for (int i = 0; i < STAR_COUNT; i++) {
            float starSeed = float(i) * 42.758;
            float starX = hash(starSeed) * float(WSX);
            float starY = hash(starSeed + 7.3219) * skyLimit;
            
            // Distance à l'étoile
            float distToStar = length(vec2(float(x) - starX, float(y) - starY));
            
            // Taille variable des étoiles
            float starRadius = 0.5 + hash(starSeed + 13.7) * 1.5;
            
            // Luminosité variable des étoiles
            float starBrightness = 0.5 + hash(starSeed + 21.13) * 0.5;
            
            // Faire scintiller les étoiles avec le temps
            float flicker = 0.7 + 0.3 * sin(float(step) * 0.05 + hash(starSeed) * 6.28);
            starBrightness *= flicker;
            
            // Dessiner l'étoile avec un dégradé doux
            if (distToStar < starRadius) {
                float starIntensity = (1.0 - distToStar / starRadius) * starBrightness;
                vec3 starColor = vec3(0.9, 0.95, 1.0) * starIntensity; // Blanc légèrement bleuté
                
                // Pour quelques étoiles, ajouter une teinte colorée
                if (hash(starSeed + 33.97) > 0.85) {
                    // Rouge, bleu ou jaune selon le hash
                    float colorType = hash(starSeed + 50.31);
                    if (colorType < 0.33) {
                        starColor = mix(starColor, vec3(1.0, 0.7, 0.7), 0.4); // Rougeâtre
                    } else if (colorType < 0.66) {
                        starColor = mix(starColor, vec3(0.7, 0.7, 1.0), 0.4); // Bleuâtre
                    } else {
                        starColor = mix(starColor, vec3(1.0, 1.0, 0.7), 0.4); // Jaunâtre
                    }
                }
                
                // Mélanger avec la couleur existante
                vec3 existingColor = vec3(
                    float((data_0[p] >> 16) & 0xFF) / 255.0,
                    float((data_0[p] >> 8) & 0xFF) / 255.0,
                    float(data_0[p] & 0xFF) / 255.0
                );
                
                vec3 finalColor = existingColor + starColor;
                finalColor = min(finalColor, vec3(1.0)); // Limiter à 1.0
                
                data_0[p] = 0xFF000000 | (int(finalColor.r * 255.0) << 16) | (int(finalColor.g * 255.0) << 8) | int(finalColor.b * 255.0);
            }
        }
    }
    
    // Zone d'herbe en bas
    else if (y >= int(grassStart)) {
        // Utiliser fbm pour créer une texture d'herbe
        float grassNoise = fbm(vec2(float(x) * 0.05, float(y) * 0.05 + float(step) * 0.001));
        
        // Couleur de base de l'herbe avec variation
        vec3 grassColor = vec3(0.05 + grassNoise * 0.04, 0.2 + grassNoise * 0.1, 0.05 + grassNoise * 0.02);
        
        // Assombrir progressivement l'herbe en s'éloignant de la route
        float distFromRoad = (float(y) - grassStart) / (float(WSX) - grassStart);
        grassColor = mix(grassColor, vec3(0.02, 0.08, 0.02), distFromRoad * 0.7);
        
        // Ajouter quelques brins d'herbe plus clairs
        if (hash(float(x) + float(y) * 73.1) > 0.97) {
            grassColor = mix(grassColor, vec3(0.15, 0.3, 0.1), 0.5);
        }
        
        data_0[p] = 0xFF000000 | (int(grassColor.r * 255.0) << 16) | (int(grassColor.g * 255.0) << 8) | int(grassColor.b * 255.0);
    }
    
    // Lune améliorée
    float moonX = float(WSX) * 0.85;
    float moonY = float(WSX) * 0.10;
    float moonRadius = float(WSX) * 0.05;
    float distToMoon = length(vec2(float(x) - moonX, float(y) - moonY));

    // Rayon normalisé pour les effets de surface
    float normalizedRadius = distToMoon / moonRadius;
    
    if (distToMoon < moonRadius) {
        // Utiliser FBM pour des détails de surface plus complexes
        vec2 moonUV = vec2(float(x) - moonX, float(y) - moonY) / moonRadius;
        
        // Créer des cratères plus détaillés
        float largeScale = fbm(moonUV * 5.0) * 0.15;  // Grandes formations
        float mediumScale = fbm(moonUV * 10.0) * 0.1; // Formations moyennes
        float smallScale = fbm(moonUV * 20.0) * 0.05; // Petits détails
        
        // Combiner les échelles pour créer une surface complexe
        float craterEffect = largeScale + mediumScale + smallScale;
        
        // Ajouter un léger effet de limbe (bord plus brillant)
        float limb = smoothstep(0.8, 1.0, normalizedRadius) * 0.15;
        
        // Créer un dégradé du centre vers les bords pour donner plus de profondeur
        float depthGradient = mix(0.15, 0.0, normalizedRadius * 0.7);
        
        // Couleur de base de la lune avec teinte légèrement bleutée
        vec3 moonBaseColor = vec3(0.95, 0.95, 0.92);
        
        // Appliquer les effets de surface et de profondeur
        vec3 moonColor = moonBaseColor - vec3(craterEffect) + vec3(limb) - vec3(depthGradient);
        
        // Assurer que les valeurs restent dans une plage valide
        moonColor = clamp(moonColor, vec3(0.0), vec3(1.0));
        
        data_0[p] = 0xFF000000 | (int(moonColor.r * 255.0) << 16) | (int(moonColor.g * 255.0) << 8) | int(moonColor.b * 255.0);
    } 
    else if (distToMoon < moonRadius * 2.0) {
        // Halo extérieur amélioré avec gradient plus doux
        float haloDistance = (distToMoon - moonRadius) / moonRadius;
        float haloIntensity = pow(1.0 - haloDistance, 2.0) * 0.4;
        
        // Faire varier légèrement la couleur du halo
        vec3 innerHaloColor = vec3(0.95, 0.95, 0.9);  // Plus proche de la couleur de la lune
        vec3 outerHaloColor = vec3(0.7, 0.75, 0.9);   // Teinte bleutée pour l'extérieur du halo
        
        // Mélanger entre les couleurs intérieures et extérieures du halo
        vec3 haloColor = mix(outerHaloColor, innerHaloColor, 1.0 - haloDistance);
        
        // Récupérer la couleur existante (ciel ou étoiles)
        vec3 existingColor = vec3(
            float((data_0[p] >> 16) & 0xFF) / 255.0,
            float((data_0[p] >> 8) & 0xFF) / 255.0,
            float(data_0[p] & 0xFF) / 255.0
        );
        
        // Mélanger avec la couleur de fond selon l'intensité
        vec3 finalHaloColor = mix(existingColor, haloColor, haloIntensity);
        
        data_0[p] = 0xFF000000 | (int(finalHaloColor.r * 255.0) << 16) | (int(finalHaloColor.g * 255.0) << 8) | int(finalHaloColor.b * 255.0);
    }

    // Nuages
    for (int i = 0; i < 5; i++) {
        float cloudSeed = float(i) * 100.0;
        float cloudX = mod(float(WSX) * hash(cloudSeed) + float(step) * 0.2, float(WSX));
        float cloudY = float(WSX) * 0.12 + float(i) * 15.0;
        float cloudW = float(WSX) * (0.2 + hash(cloudSeed + 1.0) * 0.2);
        float cloudH = float(WSX) * 0.05;
        
        // Forme du nuage avec bruit
        float cloudNoise = noise(vec2(float(x) * 0.01 + cloudSeed, float(y) * 0.01 + cloudSeed + float(step) * 0.005));
        float cloudDist = length((vec2(float(x), float(y)) - vec2(cloudX, cloudY)) / vec2(cloudW, cloudH));
        cloudDist += cloudNoise * 0.5;
        
        if (cloudDist < 1.0) {
            float cloudIntensity = (1.0 - cloudDist) * 0.3;
            float moonInfluence = 0.0;
            
            // Nuages éclairés par la lune
            float distToMoonFromCloud = length(vec2(cloudX - moonX, cloudY - moonY));
            if (distToMoonFromCloud < moonRadius * 5.0) {
                moonInfluence = max(0.0, 0.5 - distToMoonFromCloud / (moonRadius * 10.0));
            }
            
            vec3 cloudColor = mix(
                vec3(0.9, 0.9, 0.9), // Nuages clairs
                vec3(0.6, 0.6, 0.7),  // Nuages légèrement ombrés
                moonInfluence
            );
            
            vec3 existingColor = vec3(
                float((data_0[p] >> 16) & 0xFF) / 255.0,
                float((data_0[p] >> 8) & 0xFF) / 255.0,
                float(data_0[p] & 0xFF) / 255.0
            );
            
            vec3 finalColor = mix(existingColor, cloudColor, cloudIntensity);
            data_0[p] = 0xFF000000 | (int(finalColor.r * 255.0) << 16) | (int(finalColor.g * 255.0) << 8) | int(finalColor.b * 255.0);
        }
    }

    // Délimiter les voies
    float laneY2 = float(WSX) / 4.0 + 35; // Voie du haut (voitures vers la droite - rapide)
    float laneY3 = float(WSX) / 4.0 + 80; // Voie du bas (voitures vers la gauche - rapide)
    
    // Route mouillée
    if (y >= int(skyLimit) && y < int(grassStart)) {
        // Effet de route mouillée avec reflets
        vec3 roadColor = vec3(0.08, 0.08, 0.07);
        
        // Ajouter un peu de reflet pour la route mouillée
        float reflectFactor = noise(vec2(float(x) * 0.02, float(y) * 0.02)) * 0.03;
        roadColor = roadColor + vec3(reflectFactor);
        
        data_0[p] = 0xFF000000 | (int(roadColor.r * 255.0) << 16) | (int(roadColor.g * 255.0) << 8) | int(roadColor.b * 255.0);
        
        // Ligne entre les voies allant dans le même sens
        if (y > int(laneY1) + 30 && y < int(laneY1) + 33) {
            // Ligne pointillée pour voies allant dans le même sens (droite)
            if (int(float(x) / 20.0) % 2 == 0) {
                vec3 lineColor = vec3(0.8, 0.8, 0.1); // Jaune
                data_0[p] = 0xFF000000 | (int(lineColor.r * 255.0) << 16) | (int(lineColor.g * 255.0) << 8) | int(lineColor.b * 255.0);
            }
        }
        
        // Ligne centrale entre les deux sens (double ligne continue)
        if ((y > int(laneY2) + 25 && y < int(laneY2) + 28) || 
            (y > int(laneY3) - 10 && y < int(laneY3) - 7)) {
            vec3 lineColor = vec3(0.8, 0.8, 0.1); // Jaune
            data_0[p] = 0xFF000000 | (int(lineColor.r * 255.0) << 16) | (int(lineColor.g * 255.0) << 8) | int(lineColor.b * 255.0);
        }
        
        // Ligne entre les voies allant dans le même sens
        if (y > int(laneY3) + 30 && y < int(laneY3) + 33) {
            // Ligne pointillée pour voies allant dans le même sens (gauche)
            if (int(float(x) / 20.0) % 2 == 0) {
                vec3 lineColor = vec3(0.8, 0.8, 0.1); // Jaune
                data_0[p] = 0xFF000000 | (int(lineColor.r * 255.0) << 16) | (int(lineColor.g * 255.0) << 8) | int(lineColor.b * 255.0);
            }
        }
    }
    
    // Stocker les positions des voitures pour calculer les distances entre elles
    vec2 carPositions[CAR_COUNT];
    bool carDirections[CAR_COUNT];
    
    // Premier parcours pour calculer les positions
    for (int carIdx = 0; carIdx < CAR_COUNT; carIdx++) {
        // Déterminer la direction (droite ou gauche) et la voie (lente ou rapide)
        bool goingRight = (carIdx < CAR_COUNT / 2);
        carDirections[carIdx] = goingRight;
        
        // Les voitures avec indices pairs vont sur la voie "lente", les impaires sur la voie "rapide"
        bool fastLane = (carIdx % 2 == 1);
        
        // Différentes vitesses selon la voie et avec variation individuelle
        float baseSpeed = fastLane ? 1.4 : 0.8; // Les voies rapides sont 1.75x plus rapides
        float speedMultiplier = baseSpeed + hash(float(carIdx) * 10.0) * 0.3; // Variation individuelle
        
        // Position Y selon la direction et la voie
        float carPosY;
        if (goingRight) {
            carPosY = fastLane ? laneY2 - 10.0 : laneY1 - 10.0;
        } else {
            carPosY = fastLane ? laneY3 - 10.0 : laneY4 - 10.0;
        }
        
        // Position X avec décalage et direction
        float carOffset = float(carIdx % (CAR_COUNT/2)) * (float(WSX) + CAR_WIDTH) / float(CAR_COUNT/2) * 0.8;
        float carPosX;
        
        if (goingRight) {
            // Voitures allant de gauche à droite
            carPosX = mod(float(step) * SPEED * speedMultiplier + carOffset, float(WSX) + CAR_WIDTH) - CAR_WIDTH;
        } else {
            // Voitures allant de droite à gauche
            carPosX = float(WSX) - mod(float(step) * SPEED * speedMultiplier + carOffset, float(WSX) + CAR_WIDTH);
        }
        
        // Stocker la position pour les calculs de proximité
        carPositions[carIdx] = vec2(carPosX + CAR_WIDTH * 0.5, carPosY + CAR_HEIGHT * 0.5);
    }
    
    // Dessiner les voitures dans les deux sens
    for (int carIdx = 0; carIdx < CAR_COUNT; carIdx++) {
        // Récupérer les informations déjà calculées
        bool goingRight = carDirections[carIdx];
        
        // Déterminer si c'est une voiture sur voie rapide
        bool fastLane = (carIdx % 2 == 1);
        
        // Position Y selon la direction et la voie
        float carPosY;
        if (goingRight) {
            carPosY = fastLane ? laneY2 - 10.0 : laneY1 - 10.0;
        } else {
            carPosY = fastLane ? laneY3 - 10.0 : laneY4 - 10.0;
        }
        
        // Récupérer la vitesse et l'offset
        float baseSpeed = fastLane ? 1.4 : 0.8;
        float speedMultiplier = baseSpeed + hash(float(carIdx) * 10.0) * 0.3;
        float carOffset = float(carIdx % (CAR_COUNT/2)) * (float(WSX) + CAR_WIDTH) / float(CAR_COUNT/2) * 0.8;
        float carPosX;
        
        if (goingRight) {
            carPosX = mod(float(step) * SPEED * speedMultiplier + carOffset, float(WSX) + CAR_WIDTH) - CAR_WIDTH;
        } else {
            carPosX = float(WSX) - mod(float(step) * SPEED * speedMultiplier + carOffset, float(WSX) + CAR_WIDTH);
        }
        
        // Dessiner la voiture
        if (float(x) >= carPosX && float(x) < carPosX + CAR_WIDTH && 
            float(y) >= carPosY && float(y) < carPosY + CAR_HEIGHT) {
            
            // Couleur de la voiture - variée selon l'index
            vec3 carColor = vec3(0.1 + hash(float(carIdx) * 42.0) * 0.2, 
                                0.1 + hash(float(carIdx) * 17.0) * 0.1, 
                                0.2 + hash(float(carIdx) * 29.0) * 0.3);
            
            // Vitres
            if (float(x) >= carPosX + CAR_WIDTH * 0.25 && float(x) < carPosX + CAR_WIDTH * 0.75 && 
                float(y) >= carPosY && float(y) < carPosY + CAR_HEIGHT * 0.5) {
                carColor = vec3(0.2, 0.2, 0.3); // Vitres teintées
            }
            
            data_0[p] = 0xFF000000 | (int(carColor.r * 255.0) << 16) | (int(carColor.g * 255.0) << 8) | int(carColor.b * 255.0);
        }
        
        // Positions des phares et feux arrière selon la direction
        float frontLightX, rearLightX;
        if (goingRight) {
            frontLightX = carPosX + CAR_WIDTH * 0.9;
            rearLightX = carPosX + CAR_WIDTH * 0.1;
        } else {
            // Inversé pour les voitures allant vers la gauche
            frontLightX = carPosX + CAR_WIDTH * 0.1;
            rearLightX = carPosX + CAR_WIDTH * 0.9;
        }
        
        float lightY = carPosY + CAR_HEIGHT * 0.7;
        float frontLightRadius = 4.0;
        float rearLightRadius = 3.0;
        
        // Déterminer si la voiture est en croisement pour ajuster la couleur des phares
        float headlightIntensity = 1.0;
        
        // Vérifier la proximité avec les voitures venant en sens inverse
        for (int otherCarIdx = 0; otherCarIdx < CAR_COUNT; otherCarIdx++) {
            if (otherCarIdx != carIdx && carDirections[otherCarIdx] != goingRight) {
                float distBetweenCars = length(carPositions[carIdx] - carPositions[otherCarIdx]);
                bool facingEachOther = false;
                
                if (goingRight) {
                    facingEachOther = (carPositions[otherCarIdx].x > carPositions[carIdx].x);
                } else {
                    facingEachOther = (carPositions[otherCarIdx].x < carPositions[carIdx].x);
                }
                
                if (facingEachOther && distBetweenCars < 250.0) {
                    float proximityFactor = distBetweenCars / 250.0;
                    headlightIntensity = min(headlightIntensity, 0.3 + proximityFactor * 0.7);
                }
            }
        }
        
        // Phares avant (plus ou moins intenses selon le croisement)
        if (length(vec2(float(x) - frontLightX, float(y) - lightY)) < frontLightRadius) {
            // Couleur légèrement plus jaune/orange pour les feux de croisement
            vec3 fullBeamColor = vec3(1.0, 0.9, 0.7); // Jaune chaud (pleins phares)
            vec3 lowBeamColor = vec3(0.9, 0.8, 0.5);  // Plus orange (feux de croisement)
            vec3 lightColor = mix(lowBeamColor, fullBeamColor, headlightIntensity);
            
            data_0[p] = 0xFF000000 | (int(lightColor.r * 255.0) << 16) | (int(lightColor.g * 255.0) << 8) | int(lightColor.b * 255.0);
        }
        
        // Feux arrière (rouges)
        if (length(vec2(float(x) - rearLightX, float(y) - lightY)) < rearLightRadius) {
            vec3 redLight = vec3(1.0, 0.1, 0.1); // Rouge
            data_0[p] = 0xFF000000 | (int(redLight.r * 255.0) << 16) | (int(redLight.g * 255.0) << 8) | int(redLight.b * 255.0);
        }
        
        // Clignotants
        bool shouldBlink = false;
        bool blinkOn = false;
        
        // Déterminer si la voiture doit clignoter
        // Les voitures sur voie rapide qui sont sur le point de sortir vont clignoter pour indiquer un changement de voie
        if (fastLane) {
            // Probabilité que la voiture veuille changer de voie
            float changeLaneProbability = hash(float(carIdx) * 42.0 + float(step) * 0.01);
            
            // Vérifier si la voiture est sur le point de changer de voie
            if (changeLaneProbability > 0.7) {
                // Activer le clignotant
                shouldBlink = true;
                
                // Séquence de clignotement (1 seconde allumé, 1 seconde éteint)
                blinkOn = (int(float(step) / 30.0) % 2 == 0);
            }
        }
        
        if (shouldBlink && blinkOn) {
            // Position du clignotant selon la direction
            float blinkX, blinkY;
            
            if (goingRight) {
                // Clignotant à droite pour voiture allant vers la droite (veut se rabattre à droite)
                blinkX = carPosX + CAR_WIDTH * 0.9;
                blinkY = carPosY + CAR_HEIGHT * 0.5;
            } else {
                // Clignotant à gauche pour voiture allant vers la gauche (veut se rabattre à gauche)
                blinkX = carPosX + CAR_WIDTH * 0.1;
                blinkY = carPosY + CAR_HEIGHT * 0.5;
            }
            
            // Dessiner le clignotant
            if (length(vec2(float(x) - blinkX, float(y) - blinkY)) < 3.0) {
                vec3 blinkColor = vec3(1.0, 0.7, 0.0); // Orange
                data_0[p] = 0xFF000000 | (int(blinkColor.r * 255.0) << 16) | (int(blinkColor.g * 255.0) << 8) | int(blinkColor.b * 255.0);
            }
        }
        
        // Roues
        float frontWheelX, rearWheelX;
        if (goingRight) {
            frontWheelX = carPosX + CAR_WIDTH * 0.75;
            rearWheelX = carPosX + CAR_WIDTH * 0.25;
        } else {
            // Inversé pour les voitures allant vers la gauche
            frontWheelX = carPosX + CAR_WIDTH * 0.25;
            rearWheelX = carPosX + CAR_WIDTH * 0.75;
        }
        
        float wheelY = carPosY + CAR_HEIGHT;
        
        float distFrontWheel = length(vec2(float(x) - frontWheelX, float(y) - wheelY));
        float distRearWheel = length(vec2(float(x) - rearWheelX, float(y) - wheelY));
        
        if (distFrontWheel <= WHEEL_RADIUS || distRearWheel <= WHEEL_RADIUS) {
            vec3 wheelColor = vec3(0.1, 0.1, 0.1); // Noir
            data_0[p] = 0xFF000000 | (int(wheelColor.r * 255.0) << 16) | (int(wheelColor.g * 255.0) << 8) | int(wheelColor.b * 255.0);
        }
        
        // Faisceau des phares - direction dépendant du sens de circulation
        float headlightX = frontLightX;
        float headlightY = lightY;
        
        // Direction du faisceau (droite ou gauche selon le sens)
        bool beamRight = goingRight;
        
        // Déterminer si une voiture est en face (croisement)
        float beamIntensityMultiplier = 1.0; // Pleine puissance par défaut
        float detectionDistance = 250.0; // Distance à laquelle les voitures détectent celles qui arrivent en face
        
        // Vérifier la proximité avec les voitures venant en sens inverse
        for (int otherCarIdx = 0; otherCarIdx < CAR_COUNT; otherCarIdx++) {
            if (otherCarIdx != carIdx && carDirections[otherCarIdx] != goingRight) {
                // Calculer la distance entre les voitures
                float distBetweenCars = length(carPositions[carIdx] - carPositions[otherCarIdx]);
                
                // Vérifier si les voitures sont face à face (pas seulement proches)
                bool facingEachOther = false;
                
                if (goingRight) {
                    // Si la voiture actuelle va à droite, l'autre doit être devant elle
                    facingEachOther = (carPositions[otherCarIdx].x > carPositions[carIdx].x);
                } else {
                    // Si la voiture actuelle va à gauche, l'autre doit être devant elle
                    facingEachOther = (carPositions[otherCarIdx].x < carPositions[carIdx].x);
                }
                
                // Réduire l'intensité si les voitures sont proches et en face
                if (facingEachOther && distBetweenCars < detectionDistance) {
                    // Réduction progressive de l'intensité des phares à l'approche
                    float proximityFactor = distBetweenCars / detectionDistance;
                    // Réduction entre 30% (feux de croisement) et 100% (pleins phares)
                    beamIntensityMultiplier = min(beamIntensityMultiplier, 0.3 + proximityFactor * 0.7);
                }
            }
        }
        
        // Condition pour dessiner le faisceau dans la bonne direction
        if ((beamRight && float(x) > headlightX) || (!beamRight && float(x) < headlightX)) {
            float dx = float(x) - headlightX;
            float dy = float(y) - headlightY;
            
            // Inverser l'angle pour les voitures allant vers la gauche
            float angle;
            if (beamRight) {
                angle = atan(dy / dx);
            } else {
                angle = atan(dy / -dx); // Note le signe négatif pour dx
            }
            
            // Ajuster l'angle du faisceau en fonction de l'intensité (feux de croisement plus étroits et vers le bas)
            float maxAngle = mix(0.2, 0.3, beamIntensityMultiplier);
            float minAngle = mix(-0.1, -0.3, beamIntensityMultiplier);
            
            if (angle >= minAngle && angle <= maxAngle) {
                float dist = length(vec2(dx, dy));
                // Ajuster la portée en fonction de l'intensité
                float maxDist = mix(80.0, 150.0, beamIntensityMultiplier);
                
                if (dist < maxDist) {
                    float intensity = 1.0 - (dist / maxDist);
                    // Appliquer le multiplicateur d'intensité pour les croisements
                    intensity = intensity * intensity * 0.4 * beamIntensityMultiplier;
                    
                    // Couleur légèrement plus jaune pour les feux de croisement
                    vec3 beamColor = mix(vec3(1.0, 0.8, 0.5), vec3(1.0, 0.9, 0.7), beamIntensityMultiplier);
                    vec3 lightColor = beamColor * intensity;
                    
                    // Combiner la lumière avec la couleur existante
                    vec3 existingColor = vec3(
                        float((data_0[p] >> 16) & 0xFF) / 255.0,
                        float((data_0[p] >> 8) & 0xFF) / 255.0,
                        float(data_0[p] & 0xFF) / 255.0
                    );
                    
                    vec3 finalColor = existingColor + lightColor;
                    finalColor = min(finalColor, vec3(1.0)); // Limiter à 1.0
                    
                    data_0[p] = 0xFF000000 | (int(finalColor.r * 255.0) << 16) | (int(finalColor.g * 255.0) << 8) | int(finalColor.b * 255.0);
                }
            }
        }
    }
}