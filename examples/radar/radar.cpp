#define M_PI 3.14159265358979323846

// Color definitions
#define blue1 vec3(0.74, 0.0, 0.00)
#define blue2 vec3(0.87, 0.98, 1.00)
#define blue3 vec3(1.0, 1.0, 1.0)
#define blue4 vec3(0.953, 0.969, 0.89)
#define red   vec3(0.00, 0.00, 1.00)

// Helper macro
#define SMOOTH(r, R) (1.0 - smoothstep(R - 1.0, R + 1.0, r))

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
    //Calcule les coordonnées normalisées et la distance depuis le centre de l'écran (r).
    vec2 uv = vec2(float(x), float(y));
    vec2 c = vec2(float(WSX) / 2.0, float(WSY) / 2.0); // Center
    vec2 d = uv - c;
    float r = length(d);
    
    //Définit un facteur d'échelle pour adapter les éléments à la taille de l'écran et une variable du temps   
    float scale = min(float(WSX), float(WSY)) / 650.0;
    float time = float(step) * 0.01;
    
    // arriere plan noir
    vec3 finalColor = vec3(0.0);
    
    // Normalized direction (for angles)
    vec2 dn = r > 0.0 ? d / r : vec2(0.0);
    float angle = atan(dn.y, dn.x);
    float theta = 180.0 * angle / M_PI;
    
    //Dessine trois cercles concentriques de différentes tailles.
    finalColor += (SMOOTH(r - 0.5 * scale, 100.0 * scale) - SMOOTH(r + 0.5 * scale, 100.0 * scale)) * blue1;
    finalColor += (SMOOTH(r - 0.5 * scale, 165.0 * scale) - SMOOTH(r + 0.5 * scale, 165.0 * scale)) * blue1;
    finalColor += (SMOOTH(r - 1.0 * scale, 240.0 * scale) - SMOOTH(r + 1.0 * scale, 240.0 * scale)) * blue4;
    
    // Dessine un petit point au centre.
    finalColor += (SMOOTH(r - 0.5 * scale, 10.0 * scale) - SMOOTH(r + 0.5 * scale, 10.0 * scale)) * blue3;
    
    // Crée une ligne de balayage qui tourne comme un radar, avec un gradient.
    if (r < 240.0 * scale) {
        float scanAngle = 90.0 * time * M_PI / 180.0;
        vec2 scanDir = vec2(cos(scanAngle), -sin(scanAngle));
        float scanDist = dot(dn, scanDir);
        float theta0 = 90.0 * time;
        float angDiff = mod(theta + theta0, 360.0);
        float gradient = clamp(1.0 - angDiff / 90.0, 0.0, 1.0);
        float scanLine = SMOOTH(1.0 - scanDist, 0.03) * 0.75;
        finalColor += (scanLine + 0.5 * gradient) * blue3 * smoothstep(240.0 * scale, 220.0 * scale, r);
    }
    
    // Dessine un cercle extérieur avec des segments réguliers.
    if (r > 310.0 * scale && r < 316.0 * scale) {
        float segmentPattern = smoothstep(2.0, 2.1, abs(mod(theta + 2.0, 45.0) - 2.0)) * 
                              mix(0.5, 1.0, float(abs(mod(theta, 180.0) - 90.0) > 45.0));
        finalColor += segmentPattern * blue1;
    }
    
    // Crée un demi-cercle avec une ouverture qui varie avec le temps.
    if (r > 260.0 * scale && r < 264.0 * scale) {
        float opening = 0.5 + 0.2 * cos(time);
        if (abs(dn.y) > opening) {
            finalColor += 0.7 * blue3;
        }
    }


     // Dessine quatre triangles qui pulsent avec le temps autour d'un cercle invisible.
     float radius = (315.0 + 30.0 * sin(time)) * scale;
     // Right triangle
     if (d.x - radius >= -8.0 * scale && d.x - radius <= 0.0) {
         float t = 1.0 - smoothstep(7.0 * scale + d.x - radius, 9.0 * scale + d.x - radius, abs(d.y));
         finalColor += t * blue2;
     }
     // Left triangle
     if (d.x + radius >= 0.0 && d.x + radius <= 8.0 * scale) {
         float t = 1.0 - smoothstep(7.0 * scale - d.x - radius, 9.0 * scale - d.x - radius, abs(d.y));
         finalColor += t * blue2;
     }
     // Bottom triangle
     if (d.y - radius >= -8.0 * scale && d.y - radius <= 0.0) {
         float t = 1.0 - smoothstep(7.0 * scale + d.y - radius, 9.0 * scale + d.y - radius, abs(d.x));
         finalColor += t * blue2;
     }
     // Top triangle
     if (d.y + radius >= 0.0 && d.y + radius <= 8.0 * scale) {
         float t = 1.0 - smoothstep(7.0 * scale - d.y - radius, 9.0 * scale - d.y - radius, abs(d.x));
         finalColor += t * blue2;
     }
     
    // Dessine deux points mobiles qui se déplacent sur des trajectoires complexes. 
    if (r < 240.0 * scale) {
        // Blip 1
        {
            float t = time * 0.1;
            vec2 pos = c + 130.0 * scale * vec2(
                1.3 * cos(t) + 1.0 * cos(0.1 * t),
                1.0 * sin(t) + 1.4 * cos(0.1 * t)
            );
            float blipDist = length(uv - pos);
            finalColor += SMOOTH(blipDist, 3.0 * scale) * vec3(1, 1, 1);
        }
        // Blip 2 
        {
            float t = sin(0.1 * time + 7.0) + 0.2 * time;
            vec2 pos = c + 50.0 * scale * vec2(
                1.54 * cos(t) + 1.7 * cos(0.1 * t),
                1.37 * sin(t) + 1.8 * cos(0.1 * t)
            );
            float blipDist = length(uv - pos);
            float R = (8.0 + mod(87.0 * time, 80.0)) * scale;
            
            float blip = (0.5 - 0.5 * cos(30.0 * time)) * SMOOTH(blipDist, 5.0 * scale)
                + SMOOTH(6.0 * scale, blipDist) - SMOOTH(8.0 * scale, blipDist)
                + smoothstep(max(8.0 * scale, R - 20.0 * scale), R, blipDist) - SMOOTH(R, blipDist);
                
            finalColor += blip * red;
        }
    }
    
    // Dessine trois cibles fixes avec des halos lumineux.
    vec2 targetPositions[3] = vec2[](
        c + vec2(0.3, 0.2) * float(WSX) * 0.5,
        c + vec2(-0.4, -0.3) * float(WSX) * 0.5,
        c + vec2(0.1, -0.5) * float(WSX) * 0.5
    );
    
    for (int i = 0; i < 3; i++) {
        float targetDist = length(uv - targetPositions[i]);
        finalColor += red * exp(-50.0 * targetDist / scale) * 0.7;
    }
    
    int color = (int(clamp(finalColor.r, 0.0, 1.0) * 255.0) << 16) | 
                (int(clamp(finalColor.g, 0.0, 1.0) * 255.0) << 8) | 
                int(clamp(finalColor.b, 0.0, 1.0) * 255.0);
    
    data_0[p] = 0xFF000000 | color;
}