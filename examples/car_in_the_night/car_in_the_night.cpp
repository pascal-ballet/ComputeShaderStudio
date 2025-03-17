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
    float carPosY = float(WSX) / 3.0;
    
    // Nuit - fond noir avec leger bleu
    vec3 nightColor = vec3(0.02, 0.03, 0.08);
    
    // Route mouillée - gris fonce avec reflets
    vec3 roadColor = vec3(0.05, 0.05, 0.07);
    
    // Couleur de base - nuit
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

    // Route mouillée
    if (y > int(carPosY) + 30) {
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
    }
    
    // Faisceau des phares
    float headlightX = carPosX + CAR_WIDTH * 0.9;
    float headlightY = carPosY + CAR_HEIGHT * 0.7;
    
    // Forme conique du faisceau
    if (float(x) > headlightX && float(y) > int(carPosY) + 30) {
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
    
// Dessiner les voitures multiples
float carSpacing = (float(WSX) + CAR_WIDTH) / float(CAR_COUNT);
for (int carIdx = 0; carIdx < CAR_COUNT; carIdx++) {
    // Positions des voitures décalées, avec vitesses variées
    float speedMultiplier = 0.8 + hash(float(carIdx) * 10.0) * 0.6; // Vitesse entre 0.8 et 1.4 fois SPEED
    float carOffset = float(carIdx) * carSpacing / 1.5;
    float carPosX = mod(float(step) * SPEED * speedMultiplier + carOffset, float(WSX) + CAR_WIDTH) - CAR_WIDTH;
    float carPosY = float(WSX) / 3.0 - float(carIdx % 2) * 5.0; // Légère variation de hauteur
    
    // Dessiner la voiture
    if (float(x) >= carPosX && float(x) < carPosX + CAR_WIDTH && 
        float(y) >= carPosY && float(y) < carPosY + CAR_HEIGHT) {
        
        // Couleur de la voiture - variée selon l'index
        vec3 carColor = vec3(0.1 + hash(float(carIdx) * 42.0) * 0.2, 
                            0.1 + hash(float(carIdx) * 17.0) * 0.1, 
                            0.2 + hash(float(carIdx) * 29.0) * 0.3); // Variations de couleurs
        
        // Vitres
        if (float(x) >= carPosX + CAR_WIDTH * 0.25 && float(x) < carPosX + CAR_WIDTH * 0.75 && 
            float(y) >= carPosY && float(y) < carPosY + CAR_HEIGHT * 0.5) {
            carColor = vec3(0.2, 0.2, 0.3); // Vitres teintées
        }
        
        data_0[p] = 0xFF000000 | (int(carColor.r * 255.0) << 16) | (int(carColor.g * 255.0) << 8) | int(carColor.b * 255.0);
    }
    
    // Phares avant
    float frontLightX = carPosX + CAR_WIDTH * 0.9;
    float frontLightY = carPosY + CAR_HEIGHT * 0.7;
    float frontLightRadius = 4.0;
    
    if (length(vec2(float(x) - frontLightX, float(y) - frontLightY)) < frontLightRadius) {
        vec3 lightColor = vec3(1.0, 0.9, 0.7); // Jaune chaud
        data_0[p] = 0xFF000000 | (int(lightColor.r * 255.0) << 16) | (int(lightColor.g * 255.0) << 8) | int(lightColor.b * 255.0);
    }
    
    // Feu arrière
    float rearLightX = carPosX + CAR_WIDTH * 0.1;
    float rearLightY = carPosY + CAR_HEIGHT * 0.7;
    float rearLightRadius = 3.0;
    
    if (length(vec2(float(x) - rearLightX, float(y) - rearLightY)) < rearLightRadius) {
        vec3 redLight = vec3(1.0, 0.1, 0.1); // Rouge
        data_0[p] = 0xFF000000 | (int(redLight.r * 255.0) << 16) | (int(redLight.g * 255.0) << 8) | int(redLight.b * 255.0);
    }
    
    // Roues
    float frontWheelX = carPosX + CAR_WIDTH * 0.75;
    float rearWheelX = carPosX + CAR_WIDTH * 0.25;
    float wheelY = carPosY + CAR_HEIGHT;
    
    float distFrontWheel = length(vec2(float(x) - frontWheelX, float(y) - wheelY));
    float distRearWheel = length(vec2(float(x) - rearWheelX, float(y) - wheelY));
    
    if (distFrontWheel <= WHEEL_RADIUS || distRearWheel <= WHEEL_RADIUS) {
        vec3 wheelColor = vec3(0.1, 0.1, 0.1); // Noir
        data_0[p] = 0xFF000000 | (int(wheelColor.r * 255.0) << 16) | (int(wheelColor.g * 255.0) << 8) | int(wheelColor.b * 255.0);
    }
    
    // Faisceau des phares (uniquement pour la moitié des voitures pour éviter surcharge)
    //if (carIdx % 2 == 0) {
        float headlightX = carPosX + CAR_WIDTH * 0.9;
        float headlightY = carPosY + CAR_HEIGHT * 0.7;
        
        // Forme conique du faisceau
        if (float(x) > headlightX && float(y) > int(carPosY) + 30) {
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
    //}
}
}