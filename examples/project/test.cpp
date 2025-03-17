#define RADIUS 40
#define PUPIL_RADIUS 10
#define IRIS_RADIUS 25
#define DOT_RADIUS 3
#define CRESCENT_OFFSET 2.0  // Décalage pour créer l'effet de croissant
#define PI 3.14159265359  // Ajout de la définition de PI
#define SMALL_DOT_RADIUS 2.0  // Rayon du petit point
#define SMALL_DOT_OFFSET 1.0  // Distance du point par rapport au croissant
#define IRIS_BORDER 0.5  // Épaisseur de la bordure de l'iris
#define LIGHT_INTENSITY 0.4  // Intensité de la lumière directionnelle
#define BLINK_INTERVAL 200  // Intervalle de clignotement (10 secondes à 20fps)
#define BLINK_DURATION 20   // Durée du clignotement (1 seconde)
#define BLINK_SPEED 0.15    // Vitesse de propagation du noir
#define TRANSITION_WIDTH 5.0 // Largeur de la transition du noir
#define ANIMATION_INTERVAL 300  // 5 secondes à 60fps
#define PULSE_AMPLITUDE 10.0
#define PULSE_SPEED 0.08
#define RING_SPACING 20.0   // Espacement entre les cercles
#define RING_THICKNESS 4.0  // Épaisseur des cercles
#define SPACING_VARIATION 10.0
#define WAVE_SPEED 0.15     // Augmentation de la vitesse de propagation (anciennement 0.05)
#define TRANSITION_DURATION 60.0  // Durée de la transition en frames
#define MAX_TRANSITION_RADIUS 60.0  // Rayon maximum du cercle de transition
#define TRANSITION_OUT_START (ANIMATION_INTERVAL - TRANSITION_DURATION)

void main()
{
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);

    // initial background is black & opac
    if (step == 0) {
        data_0[p] = 0xFF000000;
    } else {
        float dx = float(mousex - x);
        float dy = float(mousey - y);
        float dist = sqrt(dx*dx + dy*dy);
        
        // Calcul de la lumière directionnelle
        float lightDirX = 1.0; // Direction X de la lumière (vers la gauche)
        float lightDirY = 1.0; // Direction Y de la lumière (vers le haut)
        float normalX = dx/dist;
        float normalY = dy/dist;
        float lightAngle = (lightDirX * normalX + lightDirY * normalY) * 0.5 + 0.5;
        
        // Position des points tournants
        float angle = float(step) * 0.05f; // Vitesse de rotation
        float orbit_radius = (IRIS_RADIUS + PUPIL_RADIUS) * 0.5f; // Rayon de l'orbite
        
        // Premier point
        float dot_x = mousex + orbit_radius * cos(angle);
        float dot_y = mousey + orbit_radius * sin(angle);
        float dot_dist = sqrt(pow(x - dot_x, 2) + pow(y - dot_y, 2));
        
        // Deuxième point (décalé de 120°)
        float dot2_x = mousex + orbit_radius * cos(angle + 2.0944); // 2π/3
        float dot2_y = mousey + orbit_radius * sin(angle + 2.0944);
        float dot2_dist = sqrt(pow(x - dot2_x, 2) + pow(y - dot2_y, 2));
        
        // Troisième point (décalé de 240°)
        float dot3_x = mousex + orbit_radius * cos(angle + 4.1888); // 4π/3
        float dot3_y = mousey + orbit_radius * sin(angle + 4.1888);
        float dot3_dist = sqrt(pow(x - dot3_x, 2) + pow(y - dot3_y, 2));

        // Premier croissant - décalage orienté vers l'extérieur
        float crescent1MainDist = sqrt(pow(x - dot_x, 2) + pow(y - dot_y, 2));
        float crescent1SecDist = sqrt(pow(x - (dot_x + CRESCENT_OFFSET * cos(angle + PI/2)), 2) + 
                                    pow(y - (dot_y + CRESCENT_OFFSET * sin(angle + PI/2)), 2));

        // Deuxième croissant - décalage orienté vers l'extérieur
        float crescent2MainDist = sqrt(pow(x - dot2_x, 2) + pow(y - dot2_y, 2));
        float crescent2SecDist = sqrt(pow(x - (dot2_x + CRESCENT_OFFSET * cos(angle + 2.0944 + + PI/2)), 2) + 
                                    pow(y - (dot2_y + CRESCENT_OFFSET * sin(angle + 2.0944 + + PI/2)), 2));

        // Troisième croissant - décalage orienté vers l'extérieur
        float crescent3MainDist = sqrt(pow(x - dot3_x, 2) + pow(y - dot3_y, 2));
        float crescent3SecDist = sqrt(pow(x - (dot3_x + CRESCENT_OFFSET * cos(angle + 4.1888 + + PI/2)), 2) + 
                                    pow(y - (dot3_y + CRESCENT_OFFSET * sin(angle + 4.1888 + + PI/2)), 2));
        
        // Points superposés aux croissants
        float dot1_center_x = dot_x - SMALL_DOT_OFFSET * cos(angle);
        float dot1_center_y = dot_y - SMALL_DOT_OFFSET * sin(angle);
        float small_dot1_dist = sqrt(pow(x - dot1_center_x, 2) + pow(y - dot1_center_y, 2));

        float dot2_center_x = dot2_x - SMALL_DOT_OFFSET * cos(angle + 2.0944);
        float dot2_center_y = dot2_y - SMALL_DOT_OFFSET * sin(angle + 2.0944);
        float small_dot2_dist = sqrt(pow(x - dot2_center_x, 2) + pow(y - dot2_center_y, 2));

        float dot3_center_x = dot3_x - SMALL_DOT_OFFSET * cos(angle + 4.1888);
        float dot3_center_y = dot3_y - SMALL_DOT_OFFSET * sin(angle + 4.1888);
        float small_dot3_dist = sqrt(pow(x - dot3_center_x, 2) + pow(y - dot3_center_y, 2));

        if (dist < RADIUS) {
            float z = sqrt(RADIUS*RADIUS - dist*dist);
            float normal_z = z/RADIUS;
            float light = normal_z;
            light = clamp(light * (0.7 + lightAngle * LIGHT_INTENSITY), 0.2f, 1.0f);
            
            // Alternance entre les animations
            bool usePulsingAnimation = ((step / ANIMATION_INTERVAL) % 2) == 1;
            
            if (usePulsingAnimation) {
                float d = sqrt(dx * dx + dy * dy);
                float cycleStart = float(step % ANIMATION_INTERVAL);
                
                // Gestion des transitions
                if (cycleStart < TRANSITION_DURATION) {
                    // Transition d'entrée (comme avant)
                    float transitionRadius = (cycleStart / TRANSITION_DURATION) * MAX_TRANSITION_RADIUS;
                    
                    if (d < transitionRadius) {
                        // Nouvelle animation
                        float animatedD = d - float(step) * WAVE_SPEED;
                        float rings = mod(animatedD, RING_SPACING);
                        
                        if (rings < RING_THICKNESS) {
                            data_0[p] = 0xFF000000;
                        } else {
                            int value = int(255.0f * light);
                            int red = int(180.0f * light);   // Augmenté de 128 à 180
                            int green = 0;
                            int blue = int(180.0f * light);  // Augmenté de 128 à 180
                            data_0[p] = 0xFF000000 | (red << 16) | (green << 8) | blue;
                        }
                    } else {
                        // Animation normale
                        if (dist < PUPIL_RADIUS) {
                            data_0[p] = 0xFF000000;
                        }
                        // ...reste du code de l'animation normale...
                    }
                } else if (cycleStart > TRANSITION_OUT_START) {
                    // Transition de sortie
                    float transitionOutProgress = (cycleStart - TRANSITION_OUT_START) / TRANSITION_DURATION;
                    float fadeRadius = (1.0 - transitionOutProgress) * MAX_TRANSITION_RADIUS;
                    
                    if (d < fadeRadius) {
                        // Zone encore visible de l'animation des cercles
                        float animatedD = d - float(step) * WAVE_SPEED;
                        float rings = mod(animatedD, RING_SPACING);
                        
                        if (rings < RING_THICKNESS) {
                            data_0[p] = 0xFF000000;
                        } else {
                            int value = int(255.0f * light);
                            int red = int(180.0f * light);   // Augmenté de 128 à 180
                            int green = 0;
                            int blue = int(180.0f * light);  // Augmenté de 128 à 180
                            data_0[p] = 0xFF000000 | (red << 16) | (green << 8) | blue;
                        }
                    } else {
                        // Zone qui revient à l'animation normale
                        if (dist < PUPIL_RADIUS) {
                            data_0[p] = 0xFF000000;
                        } else if (dist < IRIS_RADIUS) {
                            if ((crescent1MainDist < DOT_RADIUS && crescent1SecDist > DOT_RADIUS - 1.0) ||
                                (crescent2MainDist < DOT_RADIUS && crescent2SecDist > DOT_RADIUS - 1.0) ||
                                (crescent3MainDist < DOT_RADIUS && crescent3SecDist > DOT_RADIUS - 1.0) ||
                                small_dot1_dist < SMALL_DOT_RADIUS ||
                                small_dot2_dist < SMALL_DOT_RADIUS ||
                                small_dot3_dist < SMALL_DOT_RADIUS) {
                                data_0[p] = 0xFF000000;
                            } else if (dist > IRIS_RADIUS - IRIS_BORDER) {
                                data_0[p] = 0xFF404040;
                            } else {
                                int red = int(200.0f * light);
                                data_0[p] = 0xFF000000 | red;
                            }
                        } else {
                            int value = int(255.0f * light);
                            int red = value;
                            int green = int(value * 0.95);
                            int blue = int(value * 0.9);
                            data_0[p] = 0xFF000000 | (red << 16) | (green << 8) | blue;
                        }
                    }
                } else {
                    // Animation complète des cercles (entre les deux transitions)
                    float animatedD = d - float(step) * WAVE_SPEED;
                    float rings = mod(animatedD, RING_SPACING);
                    
                    if (rings < RING_THICKNESS) {
                        data_0[p] = 0xFF000000;
                    } else {
                        int value = int(255.0f * light);
                        int red = int(180.0f * light);   // Augmenté de 128 à 180
                        int green = 0;
                        int blue = int(180.0f * light);  // Augmenté de 128 à 180
                        data_0[p] = 0xFF000000 | (red << 16) | (green << 8) | blue;
                    }
                }
            } else {
                // Animation normale
                if (dist < PUPIL_RADIUS) {
                    data_0[p] = 0xFF000000;
                } else if (dist < IRIS_RADIUS) {
                    if ((crescent1MainDist < DOT_RADIUS && crescent1SecDist > DOT_RADIUS - 1.0) ||
                        (crescent2MainDist < DOT_RADIUS && crescent2SecDist > DOT_RADIUS - 1.0) ||
                        (crescent3MainDist < DOT_RADIUS && crescent3SecDist > DOT_RADIUS - 1.0) ||
                        small_dot1_dist < SMALL_DOT_RADIUS ||
                        small_dot2_dist < SMALL_DOT_RADIUS ||
                        small_dot3_dist < SMALL_DOT_RADIUS) {
                        data_0[p] = 0xFF000000;
                    } else if (dist > IRIS_RADIUS - IRIS_BORDER) {
                        data_0[p] = 0xFF404040;
                    } else {
                        int red = int(200.0f * light);
                        data_0[p] = 0xFF000000 | red;
                    }
                } else {
                    int value = int(255.0f * light);
                    int red = value;
                    int green = int(value * 0.95);
                    int blue = int(value * 0.9);
                    data_0[p] = 0xFF000000 | (red << 16) | (green << 8) | blue;
                }
            }
        } else {
            data_0[p] = 0xFF404040; // Fond gris moyen au lieu de gris foncé
        }
    }
}

