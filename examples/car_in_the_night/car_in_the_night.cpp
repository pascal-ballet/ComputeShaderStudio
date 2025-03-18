#define CAR_WIDTH 60.0
#define CAR_HEIGHT 30.0
#define WHEEL_RADIUS 8.0
#define SPEED 1.50
#define RAIN_DROPS 500
#define SPLASH_PARTICLES 20
#define CAR_COUNT 8  // Plus de voitures pour remplir les 4 voies
#define STAR_COUNT 200  // Nombre d'etoiles dans le ciel

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

// Fonction pour creer un bruit fractal (FBM - Fractional Brownian Motion)
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
    
    // Definir les limites des differentes zones - route descendue et ciel etoile agrandi
    float skyLimit = float(WSX) / 2.5;  // Limite entre ciel et route (plus grande portion de ciel)
    float laneY1 = skyLimit + 10.0;     // Voie du haut (voitures vers la droite - lente)
    float laneY4 = laneY1 + 115;        // Voie du bas (voitures vers la gauche - lente)
    float grassStart = laneY4 + 20.0;   // Debut de l'herbe apres la route (portion d'herbe reduite)
    
    // Ciel nocturne avec etoiles
    if (y < int(skyLimit)) {
        // Degrade pour le ciel nocturne (plus fonce en haut, plus clair vers l'horizon)
        float skyGradient = float(y) / skyLimit;
        vec3 skyColor = mix(
            vec3(0.25, 0.1, 0.05),  // Couleur du ciel en haut (bleu fonce)
            vec3(0.28, 0.15, 0.08),   // Couleur du ciel vers l'horizon (legerement plus claire)
            skyGradient
        );
        
        // Appliquer la couleur de base du ciel
        data_0[p] = 0xFF000000 | (int(skyColor.r * 255.0) << 16) | (int(skyColor.g * 255.0) << 8) | int(skyColor.b * 255.0);
        
        // Ajouter des etoiles
        for (int i = 0; i < STAR_COUNT; i++) {
            float starSeed = float(i) * 42.758;
            float starX = hash(starSeed) * float(WSX);
            float starY = hash(starSeed + 7.3219) * skyLimit;
            
            // Distance a l'etoile
            float distToStar = length(vec2(float(x) - starX, float(y) - starY));
            
            // Taille variable des etoiles
            float starRadius = 0.5 + hash(starSeed + 13.7) * 1.5;
            
            // Luminosite variable des etoiles
            float starBrightness = 0.5 + hash(starSeed + 21.13) * 0.5;
            
            // Faire scintiller les etoiles avec le temps
            float flicker = 0.7 + 0.3 * sin(float(step) * 0.05 + hash(starSeed) * 6.28);
            starBrightness *= flicker;
            
            // Dessiner l'etoile avec un degrade doux
            if (distToStar < starRadius) {
                float starIntensity = (1.0 - distToStar / starRadius) * starBrightness;
                vec3 starColor = vec3(0.9, 0.95, 1.0) * starIntensity; // Blanc legerement bleute
                
                // Pour quelques etoiles, ajouter une teinte coloree
                if (hash(starSeed + 33.97) > 0.85) {
                    // Rouge, bleu ou jaune selon le hash
                    float colorType = hash(starSeed + 50.31);
                    if (colorType < 0.33) {
                        starColor = mix(starColor, vec3(1.0, 0.7, 0.7), 0.4); // Rougeatre
                    } else if (colorType < 0.66) {
                        starColor = mix(starColor, vec3(0.7, 0.7, 1.0), 0.4); // Bleuatre
                    } else {
                        starColor = mix(starColor, vec3(1.0, 1.0, 0.7), 0.4); // Jaunatre
                    }
                }
                
                // Melanger avec la couleur existante
                vec3 existingColor = vec3(
                    float((data_0[p] >> 16) & 0xFF) / 255.0,
                    float((data_0[p] >> 8) & 0xFF) / 255.0,
                    float(data_0[p] & 0xFF) / 255.0
                );
                
                vec3 finalColor = existingColor + starColor;
                finalColor = min(finalColor, vec3(1.0)); // Limiter a 1.0
                
                data_0[p] = 0xFF000000 | (int(finalColor.r * 255.0) << 16) | (int(finalColor.g * 255.0) << 8) | int(finalColor.b * 255.0);
            }
        }
    }
    
    // Zone d'herbe en bas
    else if (y >= int(grassStart)) {
        // Utiliser fbm pour creer une texture d'herbe
        float grassNoise = fbm(vec2(float(x) * 0.05, float(y) * 0.05 + float(step) * 0.001));
        
        // Couleur de base de l'herbe avec variation
        vec3 grassColor = vec3(0.05 + grassNoise * 0.04, 0.2 + grassNoise * 0.1, 0.05 + grassNoise * 0.02);
        
        // Assombrir progressivement l'herbe en s'eloignant de la route
        float distFromRoad = (float(y) - grassStart) / (float(WSX) - grassStart);
        grassColor = mix(grassColor, vec3(0.02, 0.08, 0.02), distFromRoad * 0.7);
        
        // Ajouter quelques brins d'herbe plus clairs
        if (hash(float(x) + float(y) * 73.1) > 0.97) {
            grassColor = mix(grassColor, vec3(0.15, 0.3, 0.1), 0.5);
        }
        
        data_0[p] = 0xFF000000 | (int(grassColor.r * 255.0) << 16) | (int(grassColor.g * 255.0) << 8) | int(grassColor.b * 255.0);
    }
    
    // Lune amelioree
    float moonX = float(WSX) * 0.85;
    float moonY = float(WSX) * 0.10;
    float moonRadius = float(WSX) * 0.05;
    float distToMoon = length(vec2(float(x) - moonX, float(y) - moonY));

    // Rayon normalise pour les effets de surface
    float normalizedRadius = distToMoon / moonRadius;
    
    if (distToMoon < moonRadius) {
        // Utiliser FBM pour des details de surface plus complexes
        vec2 moonUV = vec2(float(x) - moonX, float(y) - moonY) / moonRadius;
        
        // Creer des crateres plus detailles
        float largeScale = fbm(moonUV * 5.0) * 0.15;  // Grandes formations
        float mediumScale = fbm(moonUV * 10.0) * 0.1; // Formations moyennes
        float smallScale = fbm(moonUV * 20.0) * 0.05; // Petits details
        
        // Combiner les echelles pour creer une surface complexe
        float craterEffect = largeScale + mediumScale + smallScale;
        
        // Ajouter un leger effet de limbe (bord plus brillant)
        float limb = smoothstep(0.8, 1.0, normalizedRadius) * 0.15;
        
        // Creer un degrade du centre vers les bords pour donner plus de profondeur
        float depthGradient = mix(0.15, 0.0, normalizedRadius * 0.7);
        
        // Couleur de base de la lune avec teinte legerement bleutee
        vec3 moonBaseColor = vec3(0.95, 0.95, 0.92);
        
        // Appliquer les effets de surface et de profondeur
        vec3 moonColor = moonBaseColor - vec3(craterEffect) + vec3(limb) - vec3(depthGradient);
        
        // Assurer que les valeurs restent dans une plage valide
        moonColor = clamp(moonColor, vec3(0.0), vec3(1.0));
        
        data_0[p] = 0xFF000000 | (int(moonColor.r * 255.0) << 16) | (int(moonColor.g * 255.0) << 8) | int(moonColor.b * 255.0);
    } 
    else if (distToMoon < moonRadius * 2.0) {
        // Halo exterieur ameliore avec gradient plus doux
        float haloDistance = (distToMoon - moonRadius) / moonRadius;
        float haloIntensity = pow(1.0 - haloDistance, 2.0) * 0.4;
        
        // Faire varier legerement la couleur du halo
        vec3 innerHaloColor = vec3(0.95, 0.95, 0.9);  // Plus proche de la couleur de la lune
        vec3 outerHaloColor = vec3(0.7, 0.75, 0.9);   // Teinte bleutee pour l'exterieur du halo
        
        // Melanger entre les couleurs interieures et exterieures du halo
        vec3 haloColor = mix(outerHaloColor, innerHaloColor, 1.0 - haloDistance);
        
        // Recuperer la couleur existante (ciel ou etoiles)
        vec3 existingColor = vec3(
            float((data_0[p] >> 16) & 0xFF) / 255.0,
            float((data_0[p] >> 8) & 0xFF) / 255.0,
            float(data_0[p] & 0xFF) / 255.0
        );
        
        // Melanger avec la couleur de fond selon l'intensite
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
            
            // Nuages eclaires par la lune
            float distToMoonFromCloud = length(vec2(cloudX - moonX, cloudY - moonY));
            if (distToMoonFromCloud < moonRadius * 5.0) {
                moonInfluence = max(0.0, 0.5 - distToMoonFromCloud / (moonRadius * 10.0));
            }
            
            vec3 cloudColor = mix(
                vec3(0.9, 0.9, 0.9), // Nuages clairs
                vec3(0.6, 0.6, 0.7),  // Nuages legerement ombres
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

    // Delimiter les voies - ajustees a la nouvelle position de la route
    float laneY2 = laneY1 + 35; // Voie du haut (voitures vers la droite - rapide)
    float laneY3 = laneY1 + 80; // Voie du bas (voitures vers la gauche - rapide)
    
    // Route mouillee
    if (y >= int(skyLimit) && y < int(grassStart)) {
        // Effet de route mouillee avec reflets
        vec3 roadColor = vec3(0.08, 0.08, 0.07);
        
        // Ajouter un peu de reflet pour la route mouillee
        float reflectFactor = noise(vec2(float(x) * 0.02, float(y) * 0.02)) * 0.03;
        roadColor = roadColor + vec3(reflectFactor);
        
        data_0[p] = 0xFF000000 | (int(roadColor.r * 255.0) << 16) | (int(roadColor.g * 255.0) << 8) | int(roadColor.b * 255.0);
        
        // Ligne entre les voies allant dans le meme sens
        if (y > int(laneY1) + 30 && y < int(laneY1) + 33) {
            // Ligne pointillee pour voies allant dans le meme sens (droite)
            if (int(float(x) / 20.0) % 2 == 0) {
                vec3 lineColor = vec3(0.9, 0.9, 0.9); // Blanc
                data_0[p] = 0xFF000000 | (int(lineColor.r * 255.0) << 16) | (int(lineColor.g * 255.0) << 8) | int(lineColor.b * 255.0);
            }
        }
        
        // Ligne centrale entre les deux sens (double ligne continue)
        if ((y > int(laneY2) + 25 && y < int(laneY2) + 28) || 
            (y > int(laneY3) - 10 && y < int(laneY3) - 7)) {
            vec3 lineColor = vec3(0.9, 0.9, 0.9); // Blanc
            data_0[p] = 0xFF000000 | (int(lineColor.r * 255.0) << 16) | (int(lineColor.g * 255.0) << 8) | int(lineColor.b * 255.0);
        }
        
        // Ligne entre les voies allant dans le meme sens
        if (y > int(laneY3) + 30 && y < int(laneY3) + 33) {
            // Ligne pointillee pour voies allant dans le meme sens (gauche)
            if (int(float(x) / 20.0) % 2 == 0) {
                vec3 lineColor = vec3(0.9, 0.9, 0.9); // Blanc
                data_0[p] = 0xFF000000 | (int(lineColor.r * 255.0) << 16) | (int(lineColor.g * 255.0) << 8) | int(lineColor.b * 255.0);
            }
        }
    }
    
    // Stocker les positions des voitures pour calculer les distances entre elles
    vec2 carPositions[CAR_COUNT];
    bool carDirections[CAR_COUNT];
    
    // Premier parcours pour calculer les positions
    for (int carIdx = 0; carIdx < CAR_COUNT; carIdx++) {
        // Determiner la direction (droite ou gauche) et la voie (lente ou rapide)
        bool goingRight = (carIdx < CAR_COUNT / 2);
        carDirections[carIdx] = goingRight;
        
        // Les voitures avec indices pairs vont sur la voie "lente", les impaires sur la voie "rapide"
        bool fastLane = (carIdx % 2 == 1);
        
        // Differentes vitesses selon la voie et avec variation individuelle
        float baseSpeed = fastLane ? 1.4 : 0.8; // Les voies rapides sont 1.75x plus rapides
        float speedMultiplier = baseSpeed + hash(float(carIdx) * 10.0) * 0.3; // Variation individuelle
        
        // Position Y selon la direction et la voie
        float carPosY;
        if (goingRight) {
            carPosY = fastLane ? laneY2 - 10.0 : laneY1 - 10.0;
        } else {
            carPosY = fastLane ? laneY3 - 10.0 : laneY4 - 10.0;
        }
        
        // Position X avec decalage et direction
        float carOffset = float(carIdx % (CAR_COUNT/2)) * (float(WSX) + CAR_WIDTH) / float(CAR_COUNT/2) * 0.8;
        float carPosX;
        
        if (goingRight) {
            // Voitures allant de gauche a droite
            carPosX = mod(float(step) * SPEED * speedMultiplier + carOffset, float(WSX) + CAR_WIDTH) - CAR_WIDTH;
        } else {
            // Voitures allant de droite a gauche
            carPosX = float(WSX) - mod(float(step) * SPEED * speedMultiplier + carOffset, float(WSX) + CAR_WIDTH);
        }
        
        // Stocker la position pour les calculs de proximite
        carPositions[carIdx] = vec2(carPosX + CAR_WIDTH * 0.5, carPosY + CAR_HEIGHT * 0.5);
    }
    
    // Dessiner les voitures dans les deux sens
    for (int carIdx = 0; carIdx < CAR_COUNT; carIdx++) {
        // Recuperer les informations deja calculees
        bool goingRight = carDirections[carIdx];
        
        // Determiner si c'est une voiture sur voie rapide
        bool fastLane = (carIdx % 2 == 1);
        
        // Position Y selon la direction et la voie
        float carPosY;
        if (goingRight) {
            
            carPosY = fastLane ? laneY3 - 10.0 : laneY4 - 10.0;
        } else {
            carPosY = fastLane ? laneY2 - 10.0 : laneY1 - 10.0;
        }
        
        // Recuperer la vitesse et l'offset
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
            
// Couleur de la voiture - couleurs de voitures de sport
vec3 carColor;
// Ajouter un effet de reflet selon la position sur la carrosserie
float normalizedX = (float(x) - carPosX) / CAR_WIDTH;
float normalizedY = (float(y) - carPosY) / CAR_HEIGHT;
float reflectionIntensity = 0.15 * pow(sin(normalizedX * 3.14), 2.0) * pow(sin(normalizedY * 3.14), 2.0);
carColor = carColor + vec3(reflectionIntensity);

// Limiter les valeurs de couleur pour eviter qu'elles ne depassent 1.0
carColor = min(carColor, vec3(1.0));

float carType = hash(float(carIdx) * 42.0); // Valeur aleatoire entre 0 et 1

if (carType < 0.15) {
    // Rouge Ferrari
    carColor = vec3(0.8, 0.1, 0.0);
} else if (carType < 0.3) {
    // Jaune Lamborghini
    carColor = vec3(0.95, 0.8, 0.0);
} else if (carType < 0.45) {
    // Bleu Bugatti
    carColor = vec3(0.0, 0.1, 0.6);
} else if (carType < 0.55) {
    // Vert Jaguar Racing
    carColor = vec3(0.0, 0.55, 0.27);
} else if (carType < 0.65) {
    // Orange McLaren
    carColor = vec3(1.0, 0.3, 0.0);
} else if (carType < 0.75) {
    // Gris Aston Martin
    carColor = vec3(0.4, 0.4, 0.45);
} else if (carType < 0.85) {
    // Blanc nacre Porsche
    carColor = vec3(0.95, 0.95, 0.97);
} else if (carType < 0.92) {
    // Noir brillant Mercedes AMG
    carColor = vec3(0.05, 0.05, 0.05);
} else {
    // Violet Koenigsegg
    carColor = vec3(0.35, 0.0, 0.35);
}

// Ajout d'un effet metallise leger
float metallic = 0.05 + hash(float(carIdx) * 29.0) * 0.1;
carColor = carColor + vec3(metallic);
            
            // Vitres
            if (float(x) >= carPosX + CAR_WIDTH * 0.25 && float(x) < carPosX + CAR_WIDTH * 0.75 && 
                float(y) >= carPosY && float(y) < carPosY + CAR_HEIGHT * 0.5) {
                carColor = vec3(0.2, 0.2, 0.3); // Vitres teintees
            }
            
            data_0[p] = 0xFF000000 | (int(carColor.b * 255.0) << 16) | (int(carColor.g * 255.0) << 8) | int(carColor.r * 255.0);
        }
        
        // Positions des phares et feux arriere selon la direction
        float frontLightX, rearLightX;
        if (goingRight) {
            frontLightX = carPosX + CAR_WIDTH * 0.9;
            rearLightX = carPosX + CAR_WIDTH * 0.1;
        } else {
            // Inverse pour les voitures allant vers la gauche
            frontLightX = carPosX + CAR_WIDTH * 0.1;
            rearLightX = carPosX + CAR_WIDTH * 0.9;
        }
        
        float lightY = carPosY + CAR_HEIGHT * 0.7;
        float frontLightRadius = 4.0;
        float rearLightRadius = 3.0;
        
        // Determiner si la voiture est en croisement pour ajuster la couleur des phares
        float headlightIntensity = 1.0;
        
        // Verifier la proximite avec les voitures venant en sens inverse
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
            // Couleur legerement plus jaune/orange pour les feux de croisement
            vec3 fullBeamColor = vec3(1.0, 0.9, 0.7); // Jaune chaud (pleins phares)
            vec3 lowBeamColor = vec3(0.9, 0.8, 0.5);  // Plus orange (feux de croisement)
            vec3 lightColor = mix(lowBeamColor, fullBeamColor, headlightIntensity);
            
            data_0[p] = 0xFF000000 | (int(lightColor.r * 255.0) << 16) | (int(lightColor.g * 255.0) << 8) | int(lightColor.b * 255.0);
        }
        
        // Feux arriere (rouges)
        if (length(vec2(float(x) - rearLightX, float(y) - lightY)) < rearLightRadius) {
            vec3 redLight = vec3(1.0, 0.1, 0.1); // Rouge
            data_0[p] = 0xFF000000 | (int(redLight.r * 255.0) << 16) | (int(redLight.g * 255.0) << 8) | int(redLight.b * 255.0);
        }
        
        // Clignotants
        bool shouldBlink = false;
        bool blinkOn = false;
        
        // Determiner si la voiture doit clignoter
        // Les voitures sur voie rapide qui sont sur le point de sortir vont clignoter pour indiquer un changement de voie
        if (fastLane) {
            // Probabilite que la voiture veuille changer de voie
            float changeLaneProbability = hash(float(carIdx) * 42.0 + float(step) * 0.01);
            
            // Verifier si la voiture est sur le point de changer de voie
            if (changeLaneProbability > 0.7) {
                // Activer le clignotant
                shouldBlink = true;
                
                // Sequence de clignotement (1 seconde allume, 1 seconde eteint)
                blinkOn = (int(float(step) / 30.0) % 2 == 0);
            }
        }
        
        if (shouldBlink && blinkOn) {
            // Position du clignotant selon la direction
            float blinkX, blinkY;
            
            if (goingRight) {
                // Clignotant a droite pour voiture allant vers la droite (veut se rabattre a droite)
                blinkX = carPosX + CAR_WIDTH * 0.9;
                blinkY = carPosY + CAR_HEIGHT * 0.5;
            } else {
                // Clignotant a gauche pour voiture allant vers la gauche (veut se rabattre a gauche)
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
            // Inverse pour les voitures allant vers la gauche
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
        
        // Faisceau des phares - direction dependant du sens de circulation
        float headlightX = frontLightX;
        float headlightY = lightY;
        
        // Direction du faisceau (droite ou gauche selon le sens)
        bool beamRight = goingRight;
        
        // Determiner si une voiture est en face (croisement)
        float beamIntensityMultiplier = 2.0; // Pleine puissance par defaut
        float detectionDistance = 250.0; // Distance a laquelle les voitures detectent celles qui arrivent en face
        
        // Verifier la proximite avec les voitures venant en sens inverse
        for (int otherCarIdx = 0; otherCarIdx < CAR_COUNT; otherCarIdx++) {
            if (otherCarIdx != carIdx && carDirections[otherCarIdx] != goingRight) {
                // Calculer la distance entre les voitures
                float distBetweenCars = length(carPositions[carIdx] - carPositions[otherCarIdx]);
                
                // Verifier si les voitures sont face a face (pas seulement proches)
                bool facingEachOther = false;
                
                if (goingRight) {
                    // Si la voiture actuelle va a droite, l'autre doit etre devant elle
                    facingEachOther = (carPositions[otherCarIdx].x > carPositions[carIdx].x);
                } else {
                    // Si la voiture actuelle va a gauche, l'autre doit etre devant elle
                    facingEachOther = (carPositions[otherCarIdx].x < carPositions[carIdx].x);
                }
                
                // Reduire l'intensite si les voitures sont proches et en face
                if (facingEachOther && distBetweenCars < detectionDistance) {
                    // Reduction progressive de l'intensite des phares a l'approche
                    float proximityFactor = distBetweenCars / detectionDistance;
                    // Reduction entre 30% (feux de croisement) et 100% (pleins phares)
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
                angle = atan(dy / -dx); // Note le signe negatif pour dx
            }
            
            // Ajuster l'angle du faisceau en fonction de l'intensite (feux de croisement plus etroits et vers le bas)
            float maxAngle = mix(0.2, 0.3, beamIntensityMultiplier);
            float minAngle = mix(-0.1, -0.3, beamIntensityMultiplier);
            
            if (angle >= minAngle && angle <= maxAngle) {
                float dist = length(vec2(dx, dy));
                // Ajuster la portee en fonction de l'intensite
                float maxDist = mix(80.0, 150.0, beamIntensityMultiplier);
                
                if (dist < maxDist) {
                    float intensity = 1.0 - (dist / maxDist);
                    // Appliquer le multiplicateur d'intensite pour les croisements
                    intensity = intensity * intensity * 0.4 * beamIntensityMultiplier;
                    
                    // Couleur legerement plus jaune pour les feux de croisement
                    vec3 beamColor = mix(vec3(1.0, 0.8, 0.5), vec3(1.0, 0.9, 0.7), beamIntensityMultiplier);
                    vec3 lightColor = beamColor * intensity;
                    
                    // Combiner la lumiere avec la couleur existante
                    vec3 existingColor = vec3(
                        float((data_0[p] >> 16) & 0xFF) / 255.0,
                        float((data_0[p] >> 8) & 0xFF) / 255.0,
                        float(data_0[p] & 0xFF) / 255.0
                    );
                    
                    vec3 finalColor = existingColor + lightColor;
                    finalColor = min(finalColor, vec3(1.0));
                    
                    data_0[p] = 0xFF000000 | (int(finalColor.r * 255.0) << 16) | (int(finalColor.g * 255.0) << 8) | int(finalColor.b * 255.0);
                }
            }
        }
    }
}