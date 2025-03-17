#define CAR_WIDTH 60.0
#define CAR_HEIGHT 30.0
#define WHEEL_RADIUS 8.0
#define SPEED 2.0
#define RAIN_DROPS 500
#define SPLASH_PARTICLES 20

float hash(float n) { return fract(sin(n) * 43758.5453123); }

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
    
    // Dessiner la voiture
    if (float(x) >= carPosX && float(x) < carPosX + CAR_WIDTH && 
        float(y) >= carPosY && float(y) < carPosY + CAR_HEIGHT) {
        
        // Couleur de la voiture - bleu foncé
        vec3 carColor = vec3(0.1, 0.1, 0.3);
        
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
    
    // Ligne médiane de la route - moins visible dans la nuit sauf sous les phares
    if (y > int(carPosY) + 40 && y < int(carPosY) + 43 && int(float(x) + float(step) * SPEED) % 70 < 40) {
        float distToLight = length(vec2(float(x) - headlightX, float(y) - headlightY));
        float visibility = 0.2;
        
        if (float(x) > headlightX) {
            visibility += 1.0 / (1.0 + distToLight * 0.03);
        }
        
        vec3 lineColor = vec3(0.8, 0.8, 0.5) * min(visibility, 1.0);
        data_0[p] = 0xFF000000 | (int(lineColor.r * 255.0) << 16) | (int(lineColor.g * 255.0) << 8) | int(lineColor.b * 255.0);
    }
    
    // Gouttes de pluie
    for (int i = 0; i < 10; i++) {
        float seed = hash(float(i) + float(x) * 0.01 + float(y) * 0.01);
        float rainX = mod(float(x) + seed * 1000.0 + float(step) * (5.0 + seed * 10.0), float(WSX));
        float rainY = mod(float(y) + seed * 1000.0 + float(step) * (15.0 + seed * 10.0), float(int(WSX) * 3 / 4));
        
        if (abs(float(x) - rainX) < 1.0 && abs(float(y) - rainY) < 3.0) {
            float distToLight = length(vec2(rainX - headlightX, rainY - headlightY));
            float visibility = 0.2;
            
            if (rainX > headlightX) {
                visibility += 1.0 / (1.0 + distToLight * 0.03);
            }
            
            vec3 rainColor = vec3(0.7, 0.7, 0.9) * min(visibility, 1.0);
            data_0[p] = 0xFF000000 | (int(rainColor.r * 255.0) << 16) | (int(rainColor.g * 255.0) << 8) | int(rainColor.b * 255.0);
        }
    }
    
    // Éclaboussures près des roues
    if (y > int(wheelY) && y < int(wheelY) + 15) {
        // Près de la roue avant
        if (abs(float(x) - frontWheelX) < WHEEL_RADIUS * 1.5) {
            float splashSeed = hash(float(x) + float(y) + float(step));
            if (splashSeed > 0.95) {
                float splashIntensity = 0.5 + splashSeed * 0.5;
                vec3 splashColor = vec3(0.8, 0.8, 0.9) * splashIntensity;
                data_0[p] = 0xFF000000 | (int(splashColor.r * 255.0) << 16) | (int(splashColor.g * 255.0) << 8) | int(splashColor.b * 255.0);
            }
        }
        
        // Près de la roue arrière
        if (abs(float(x) - rearWheelX) < WHEEL_RADIUS * 1.5) {
            float splashSeed = hash(float(x) + float(y) + float(step) + 100.0);
            if (splashSeed > 0.95) {
                float splashIntensity = 0.5 + splashSeed * 0.5;
                vec3 splashColor = vec3(0.8, 0.8, 0.9) * splashIntensity;
                data_0[p] = 0xFF000000 | (int(splashColor.r * 255.0) << 16) | (int(splashColor.g * 255.0) << 8) | int(splashColor.b * 255.0);
            }
        }
    }
}