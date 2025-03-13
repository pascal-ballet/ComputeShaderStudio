float rand(uint x, uint y) {
    return fract(sin(float(x) * 12.9898 + float(y) * 78.233) * 43758.5453);
}
void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    // Definition des couleurs
    int WHITE = 0xFFFFFFFF; // Fond blanc
    int BLACK = 0xFF000000; // Contour noir

    // Definir le centre du cercle et son rayon
    int cx = int(WSX) / 2;
    int cy = int(WSY) / 2;
    
    // Calculer la distance au centre
    int dx = int(x) - cx;
    int dy = int(y) - cy;
    int distance_squared = dx * dx + dy * dy;
    
    // Calculer l'angle du pixel par rapport au centre
    float angle = atan(float(dy), float(dx));
    if (angle < 0.0) angle += 3.14159 * 2.0;
    
    // Convertir l'angle en valeur normalisée (0-1)
    float angle_normalized = angle / (3.14159 * 2.0);
    
    // Paramètres du labyrinthe
    int num_circles = 20;                // Nombre de cercles concentriques
    int max_radius = 200;               // Rayon du cercle le plus grand
    int min_radius = 40;                // Rayon du cercle le plus petit
    int thickness = 2;                  // Épaisseur des cercles
    int frames_per_circle = 30;        // Frames pour dessiner un cercle complet
    
    // Calculer le rayon de chaque cercle
    float radius_step = float(max_radius - min_radius) / float(num_circles - 1);
    
    // Animation basée sur step
    int current_step = step % (frames_per_circle * num_circles);
    int current_circle = current_step / frames_per_circle;
    int circle_progress = current_step % frames_per_circle;
    
    // Calculer les valeurs de completion pour chaque cercle (une seule fois)
    float completion_values[20]; // Augmenté à 20 pour supporter jusqu'à 20 cercles
    float start_angles[20]; // Augmenté à 20 pour supporter jusqu'à 20 cercles
    
    // Limiter le nombre de cercles pour éviter les dépassements de tableau
    int safe_num_circles = min(num_circles, 20);
    
    for (int i = 0; i < safe_num_circles; i++) {
        completion_values[i] = 0.80 + rand(i, i) * (0.95 - 0.80);
        
        // Le premier cercle commence à 0
        if (i == 0) {
            start_angles[i] = 0.0;
        } 
        // Les cercles suivants commencent là où le précédent s'arrête
        else {
            // Si les deux cercles tournent dans le même sens
            if (i % 2 == (i-1) % 2) {
                start_angles[i] = completion_values[i-1];
            } 
            // Si les cercles tournent dans des sens opposés
            else {
                start_angles[i] = 1.0 - completion_values[i-1];
            }
        }
    }
    
    // Initialiser la couleur du pixel à blanc (fond)
    data_0[p] = WHITE;
    
    // Pour chaque cercle déjà commencé
    for (int i = 0; i <= current_circle && i < safe_num_circles; i++) {
        // Calculer le rayon du cercle actuel
        float current_radius = float(max_radius) - float(i) * radius_step;
        
        // Calculer les limites du cercle
        float inner_radius_sq = (current_radius - thickness) * (current_radius - thickness);
        float outer_radius_sq = (current_radius + thickness) * (current_radius + thickness);
        
        // Vérifier si le pixel est sur ce cercle
        if (distance_squared >= inner_radius_sq && distance_squared <= outer_radius_sq) {
            // Calculer la progression de l'animation pour ce cercle
            float max_angle = completion_values[i];
            float start_angle = start_angles[i];
            
            // Si c'est le cercle en cours d'animation
            if (i == current_circle) {
                // Calculer la progression actuelle proportionnellement
                float progress = float(circle_progress) / float(frames_per_circle);
                // Ajuster max_angle en fonction de la progression
                max_angle = start_angle + progress * (max_angle - start_angle);
            }
            
            // Alterner le sens de rotation pour chaque cercle
            float adjusted_angle;
            if (i % 2 == 0) {
                // Sens horaire
                adjusted_angle = angle_normalized;
            } else {
                // Sens anti-horaire
                adjusted_angle = 1.0 - angle_normalized;
            }
            
            // Dessiner le cercle si l'angle est dans la plage animée
            if (adjusted_angle >= start_angle && adjusted_angle <= max_angle) {
                data_0[p] = BLACK;
            }
        }
    }
    
    // Tracer des traits entre les cercles (séparé de la boucle principale pour éviter les conflits)
    for (int i = 0; i < current_circle && i < safe_num_circles - 1; i++) {
        // Calculer les rayons des cercles
        float current_radius = float(max_radius) - float(i) * radius_step;
        float next_radius = float(max_radius) - float(i+1) * radius_step;
        
        // Déterminer les angles de fin et de début
        float end_angle, next_start_angle;
        
        // Angle de fin du cercle actuel (en radians)
        if (i % 2 == 0) {
            // Cercle actuel tourne dans le sens horaire
            end_angle = completion_values[i] * 2.0 * 3.14159;
        } else {
            // Cercle actuel tourne dans le sens anti-horaire
            end_angle = (1.0 - completion_values[i]) * 2.0 * 3.14159;
        }
        
        // Angle de début du cercle suivant (en radians)
        if ((i+1) % 2 == 0) {
            // Cercle suivant tourne dans le sens horaire
            next_start_angle = start_angles[i+1] * 2.0 * 3.14159;
        } else {
            // Cercle suivant tourne dans le sens anti-horaire
            next_start_angle = (1.0 - start_angles[i+1]) * 2.0 * 3.14159;
        }
        
        // Calculer les coordonnées des points de connexion
        float end_x = float(cx) + cos(end_angle) * current_radius;
        float end_y = float(cy) + sin(end_angle) * current_radius;
        
        float start_x = float(cx) + cos(next_start_angle) * next_radius;
        float start_y = float(cy) + sin(next_start_angle) * next_radius;
        
        // Calculer la distance du pixel actuel à la ligne
        float px = float(x);
        float py = float(y);
        
        // Vecteur de la ligne
        float line_dx = start_x - end_x;
        float line_dy = start_y - end_y;
        float line_length_sq = line_dx * line_dx + line_dy * line_dy;
        
        // Projection du pixel sur la ligne
        float t = max(0.0, min(1.0, ((px - end_x) * line_dx + (py - end_y) * line_dy) / line_length_sq));
        
        // Point le plus proche sur la ligne
        float closest_x = end_x + t * line_dx;
        float closest_y = end_y + t * line_dy;
        
        // Distance au carré du pixel au point le plus proche
        float dist_sq = (px - closest_x) * (px - closest_x) + (py - closest_y) * (py - closest_y);
        
        // Si le pixel est assez proche de la ligne, le colorier
        if (dist_sq <= float(thickness * thickness)) {
            data_0[p] = BLACK;
        }
    }
}
