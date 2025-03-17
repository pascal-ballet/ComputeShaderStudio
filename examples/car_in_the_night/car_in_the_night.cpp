#define CAR_WIDTH 60.0
#define CAR_HEIGHT 30.0
#define WHEEL_RADIUS 8.0
#define SPEED 2.0
#define RAIN_DROPS 500
#define SPLASH_PARTICLES 20
#define CAR_COUNT 8

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

void main() {
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);
    
    // Position de la voiture basee sur le temps
    float carPosX = mod(float(step) * SPEED, float(WSX) + CAR_WIDTH) - CAR_WIDTH;
    
    // Réduire la hauteur de la route - modification ici
    float roadStart = float(WSX) / 2.2; // Position plus haute pour la route
    float carPosY = roadStart - CAR_HEIGHT - 10.0; // Ajuster la position des voitures
    
    // Nuit - fond bleu foncé (corrigé)
    vec3 nightColor = vec3(0.25, 0.1, 0.05); // En BGR pour obtenir un bleu foncé
    // Route mouillée - gris fonce avec reflets
    vec3 roadColor = vec3(0.08, 0.08, 0.07);
    
    // Couleur de base - nuit (s'assurer que c'est bien bleu foncé)
    data_0[p] = 0xFF000000 | (int(nightColor.r * 255.0) << 16) | (int(nightColor.g * 255.0) << 8) | int(nightColor.b * 255.0);
    
// Lune
float moonX = float(WSX) * 0.85;
float moonY = float(WSX) * 0.10;
float moonRadius = float(WSX) * 0.05;
float distToMoon = length(vec2(float(x) - moonX, float(y) - moonY));

if (distToMoon < moonRadius) {
    // Créer un léger cratère sur la lune
    float crater = noise(vec2(float(x), float(y)) * 0.1) * 0.2;
    vec3 moonColor = vec3(0.95, 0.95, 0.85) - vec3(crater);
    data_0[p] = 0xFF000000 | (int(moonColor.r * 255.0) << 16) | (int(moonColor.g * 255.0) << 8) | int(moonColor.b * 255.0);
    

} else if (distToMoon < moonRadius * 1.5) {
    // Halo extérieur
    float haloIntensity = 1.0 - ((distToMoon - moonRadius) / (moonRadius * 0.5));
    haloIntensity = haloIntensity * 0.3; // Atténuer l'intensité
    vec3 haloColor = mix(nightColor, vec3(0.9, 0.9, 0.8), haloIntensity);
    data_0[p] = 0xFF000000 | (int(haloColor.r * 255.0) << 16) | (int(haloColor.g * 255.0) << 8) | int(haloColor.b * 255.0);
}

// Nuages
for (int i = 0; i < 5; i++) {
    float cloudSeed = float(i) * 100.0;
    float cloudX = mod(float(WSX) * hash(cloudSeed) + float(step) * 0.2, float(WSX));
    float cloudY = float(WSX) * 0.2 + float(i) * 15.0;
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

    // Route mouillée - modifier cette section pour réduire la hauteur
    if (y > roadStart) {
        // Effet de route mouillée avec reflets
        float reflectionFactor = 0.0;
        
        // Reflets de la lumière des phares sur la route
        float distToFrontLight = length(vec2(float(x) - (carPosX + CAR_WIDTH * 0.9), float(y) - (carPosY + CAR_HEIGHT * 0.7)));
        float frontLightFalloff = 1.0 / (1.0 + distToFrontLight * 0.015);
        
        // Intensité des reflets uniquement devant la voiture
        if (float(x) > carPosX + CAR_WIDTH * 0.9) {
            reflectionFactor = max(0.0, frontLightFalloff * 0.5 - 0.02);
        }
        
        // Couleur de la route avec reflets
        vec3 finalRoadColor = roadColor + vec3(1.0, 0.8, 0.5) * reflectionFactor;
        
        data_0[p] = 0xFF000000 | (int(finalRoadColor.r * 255.0) << 16) | (int(finalRoadColor.g * 255.0) << 8) | int(finalRoadColor.b * 255.0);
        
        // Ligne blanche discontinue au milieu de la route
        float lineY = roadStart + (float(WSX) - roadStart) * 0.5; // Milieu de la route
        float lineThickness = 2.0;
        
        if (abs(float(y) - lineY) < lineThickness) {
            // Ligne discontinue - alternance de segments blancs et vides
            float lineSegment = mod(float(x) + float(step) * SPEED, 40.0);
            
            if (lineSegment < 20.0) {
                // Partie visible de la ligne
                vec3 lineColor = vec3(0.9, 0.9, 0.9); // Blanc
                data_0[p] = 0xFF000000 | (int(lineColor.r * 255.0) << 16) | (int(lineColor.g * 255.0) << 8) | int(lineColor.b * 255.0);
            }
        }
    }
    
    // Faisceau des phares
    float headlightX = carPosX + CAR_WIDTH * 0.9;
    float headlightY = carPosY + CAR_HEIGHT * 0.7;
    
    // Forme conique du faisceau
    if (float(x) > headlightX && float(y) > roadStart) {
        float angle = atan(float(y - int(headlightY)) / float(x - int(headlightX)));
        float maxAngle = 0.3;
        float minAngle = -0.3;
        
        if (angle >= minAngle && angle <= maxAngle) {
            float dist = length(vec2(float(x) - headlightX, float(y) - headlightY));
            float maxDist = 250.0;
            
            if (dist < maxDist) {
                float intensity = 1.0 - (dist / maxDist);
                intensity = intensity * intensity * 0.6; // Atténuation quadratique
                
                vec3 lightColor = vec3(1.0, 0.9, 0.7) * intensity;
                
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

// Panneau routier indiquant "Brest"
float signX = float(WSX) * 0.6;
float signY = float(WSX) * 0.25;
float signWidth = float(WSX) * 0.15;
float signHeight = float(WSX) * 0.05;

// Vérifier si le pixel est dans la zone du panneau
if (float(x) >= signX && float(x) <= signX + signWidth && 
    float(y) >= signY && float(y) <= signY + signHeight) {
    
    // Fond du panneau (vert foncé)
    vec3 signColor = vec3(0.0, 0.25, 0.0);
    
    // Bordure blanche
    float borderSize = 2.0;
    if (float(x) < signX + borderSize || float(x) > signX + signWidth - borderSize ||
        float(y) < signY + borderSize || float(y) > signY + signHeight - borderSize) {
        signColor = vec3(0.9, 0.9, 0.9);
    }
    
    // Zone de texte "Brest"
    if (float(x) >= signX + signWidth * 0.15 && float(x) <= signX + signWidth * 0.85 && 
        float(y) >= signY + signHeight * 0.25 && float(y) <= signY + signHeight * 0.75) {
        
        // Dessiner les lettres "BREST" (version très simplifiée)
        float textPosX = (float(x) - (signX + signWidth * 0.15)) / (signWidth * 0.7);
        float charWidth = 0.18; // Largeur d'un caractère en pourcentage de la zone de texte
        
        if (textPosX < charWidth) {
            // 'B'
            float relX = (textPosX / charWidth) * 2.0 - 1.0;
            float relY = ((float(y) - (signY + signHeight * 0.25)) / (signHeight * 0.5)) * 2.0 - 1.0;
            float bDist = min(length(vec2(relX - 0.5, relY - 0.5)) - 0.5, 
                             length(vec2(relX - 0.5, relY + 0.5)) - 0.5);
            if (relX < 0.0 || bDist < 0.4) signColor = vec3(1.0, 1.0, 1.0);
        } 
        else if (textPosX < charWidth * 2.0) {
            // 'R'
            float relX = ((textPosX - charWidth) / charWidth) * 2.0 - 1.0;
            float relY = ((float(y) - (signY + signHeight * 0.25)) / (signHeight * 0.5)) * 2.0 - 1.0;
            if (relX < 0.0 || (relY < 0.0 && length(vec2(relX - 0.5, relY + 0.5)) - 0.5 < 0.4) 
                || (relY >= 0.0 && relX < 0.5 + relY * 0.5)) signColor = vec3(1.0, 1.0, 1.0);
        }
        else if (textPosX < charWidth * 3.0) {
            // 'E'
            float relX = ((textPosX - charWidth * 2.0) / charWidth) * 2.0 - 1.0;
            float relY = ((float(y) - (signY + signHeight * 0.25)) / (signHeight * 0.5)) * 2.0 - 1.0;
            if (relX < 0.0 || abs(relY) > 0.8 || abs(relY) < 0.2) signColor = vec3(1.0, 1.0, 1.0);
        }
        else if (textPosX < charWidth * 4.0) {
            // 'S'
            float relX = ((textPosX - charWidth * 3.0) / charWidth) * 2.0 - 1.0;
            float relY = ((float(y) - (signY + signHeight * 0.25)) / (signHeight * 0.5)) * 2.0 - 1.0;
            float sDist = abs(relY) - 0.8;
            if (abs(relY) > 0.8 || abs(relY) < 0.2 || 
                (relY > 0.0 && relX < 0.0) || 
                (relY < 0.0 && relX > 0.0)) signColor = vec3(1.0, 1.0, 1.0);
        }
        else if (textPosX < charWidth * 5.0) {
            // 'T'
            float relX = ((textPosX - charWidth * 4.0) / charWidth) * 2.0 - 1.0;
            float relY = ((float(y) - (signY + signHeight * 0.25)) / (signHeight * 0.5)) * 2.0 - 1.0;
            if (abs(relY) > 0.8 || abs(relX) < 0.3) signColor = vec3(1.0, 1.0, 1.0);
        }
    }
    
    // Applique la couleur du panneau au pixel
    data_0[p] = 0xFF000000 | (int(signColor.r * 255.0) << 16) | (int(signColor.g * 255.0) << 8) | int(signColor.b * 255.0);
    
    // Effet de réflexion des phares sur le panneau
    for (int carIdx = 0; carIdx < CAR_COUNT; carIdx++) {
        float speedMultiplier = 0.8 + hash(float(carIdx) * 10.0) * 0.6;
        float carOffset = float(carIdx) * ((float(WSX) + CAR_WIDTH) / float(CAR_COUNT)) / 1.5;
        float thisCar_X = mod(float(step) * SPEED * speedMultiplier + carOffset, float(WSX) + CAR_WIDTH) - CAR_WIDTH;
        
        // Si une voiture est proche du panneau (et venant de la gauche)
        if (thisCar_X < signX && thisCar_X > signX - 150.0) {
            float distToHeadlight = length(vec2(float(x) - (thisCar_X + CAR_WIDTH * 0.9), float(y) - (carPosY + CAR_HEIGHT * 0.7)));
            float reflectionIntensity = 5.0 / (1.0 + distToHeadlight * 0.05);
            reflectionIntensity = min(reflectionIntensity * 0.3, 0.8); // Limiter l'intensité
            
            vec3 reflectionColor = mix(signColor, vec3(1.0, 0.95, 0.8), reflectionIntensity);
            data_0[p] = 0xFF000000 | (int(reflectionColor.r * 255.0) << 16) | (int(reflectionColor.g * 255.0) << 8) | int(reflectionColor.b * 255.0);
        }
    }
    
    // Support du panneau
    if (float(x) >= signX + signWidth * 0.45 && float(x) <= signX + signWidth * 0.55 && 
        float(y) > signY + signHeight && float(y) < roadStart) {
        vec3 poleColor = vec3(0.5, 0.5, 0.5); // Gris métallique
        data_0[p] = 0xFF000000 | (int(poleColor.r * 255.0) << 16) | (int(poleColor.g * 255.0) << 8) | int(poleColor.b * 255.0);
    }
}
    
// Dessiner les voitures multiples
float carSpacing = (float(WSX) + CAR_WIDTH) / float(CAR_COUNT);
for (int carIdx = 0; carIdx < CAR_COUNT; carIdx++) {
    // Positions des voitures décalées, avec vitesses variées
    float speedMultiplier = 0.8 + hash(float(carIdx) * 10.0) * 0.6; // Vitesse entre 0.8 et 1.4 fois SPEED
    float carOffset = float(carIdx) * carSpacing / 1.5;
    float thisCar_X = mod(float(step) * SPEED * speedMultiplier + carOffset, float(WSX) + CAR_WIDTH) - CAR_WIDTH;
    float thisCar_Y = carPosY - float(carIdx % 2) * 5.0; // Légère variation de hauteur
    
    // Dessiner la voiture
    if (float(x) >= thisCar_X && float(x) < thisCar_X + CAR_WIDTH && 
        float(y) >= thisCar_Y && float(y) < thisCar_Y + CAR_HEIGHT) {
        
        // Couleur de la voiture - variée selon l'index
        vec3 carColor = vec3(0.1 + hash(float(carIdx) * 42.0) * 0.2, 
                            0.1 + hash(float(carIdx) * 17.0) * 0.1, 
                            0.2 + hash(float(carIdx) * 29.0) * 0.3); // Variations de couleurs
        
        // Vitres
        if (float(x) >= thisCar_X + CAR_WIDTH * 0.25 && float(x) < thisCar_X + CAR_WIDTH * 0.75 && 
            float(y) >= thisCar_Y && float(y) < thisCar_Y + CAR_HEIGHT * 0.5) {
            carColor = vec3(0.2, 0.2, 0.3); // Vitres teintées
        }
        
        data_0[p] = 0xFF000000 | (int(carColor.r * 255.0) << 16) | (int(carColor.g * 255.0) << 8) | int(carColor.b * 255.0);
    }
    
    // Phares avant
    float frontLightX = thisCar_X + CAR_WIDTH * 0.9;
    float frontLightY = thisCar_Y + CAR_HEIGHT * 0.7;
    float frontLightRadius = 4.0;
    
    if (length(vec2(float(x) - frontLightX, float(y) - frontLightY)) < frontLightRadius) {
        vec3 lightColor = vec3(1.0, 0.9, 0.7); // Jaune chaud
        data_0[p] = 0xFF000000 | (int(lightColor.r * 255.0) << 16) | (int(lightColor.g * 255.0) << 8) | int(lightColor.b * 255.0);
    }
    
    // Feu arrière
    float rearLightX = thisCar_X + CAR_WIDTH * 0.1;
    float rearLightY = thisCar_Y + CAR_HEIGHT * 0.7;
    float rearLightRadius = 3.0;
    
    if (length(vec2(float(x) - rearLightX, float(y) - rearLightY)) < rearLightRadius) {
        vec3 redLight = vec3(1.0, 0.1, 0.1); // Rouge
        data_0[p] = 0xFF000000 | (int(redLight.r * 255.0) << 16) | (int(redLight.g * 255.0) << 8) | int(redLight.b * 255.0);
    }
    
    // Roues
    float frontWheelX = thisCar_X + CAR_WIDTH * 0.75;
    float rearWheelX = thisCar_X + CAR_WIDTH * 0.25;
    float wheelY = thisCar_Y + CAR_HEIGHT;
    
    float distFrontWheel = length(vec2(float(x) - frontWheelX, float(y) - wheelY));
    float distRearWheel = length(vec2(float(x) - rearWheelX, float(y) - wheelY));
    
    if (distFrontWheel <= WHEEL_RADIUS || distRearWheel <= WHEEL_RADIUS) {
        vec3 wheelColor = vec3(0.1, 0.1, 0.1); // Noir
        data_0[p] = 0xFF000000 | (int(wheelColor.r * 255.0) << 16) | (int(wheelColor.g * 255.0) << 8) | int(wheelColor.b * 255.0);
    }
    
    // Faisceau des phares
    float headlightX = thisCar_X + CAR_WIDTH * 0.9;
    float headlightY = thisCar_Y + CAR_HEIGHT * 0.7;
    
    // Forme conique du faisceau
    if (float(x) > headlightX && float(y) > roadStart) {
        float angle = atan(float(y - int(headlightY)) / float(x - int(headlightX)));
        float maxAngle = 0.3;
        float minAngle = -0.3;
        
        if (angle >= minAngle && angle <= maxAngle) {
            float dist = length(vec2(float(x) - headlightX, float(y) - headlightY));
            float maxDist = 250.0;
            
            if (dist < maxDist) {
                // Intensité réduite pour les voitures multiples
                float intensity = 1.0 - (dist / maxDist);
                intensity = intensity * intensity * 0.3; // Atténuation plus forte
                
                vec3 lightColor = vec3(1.0, 0.9, 0.7) * intensity;
                
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