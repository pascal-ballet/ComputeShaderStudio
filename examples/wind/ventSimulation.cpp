void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    
    // Position et taille de l'obstacle (cercle au centre)
    vec2 center = vec2(float(WSX) * 0.5, float(WSY) * 0.5);
    float radius = float(min(WSX, WSY)) * 0.1; // 10% de la taille de l'écran
    
    // Position actuelle
    vec2 pos = vec2(float(x), float(y));
    
    // Vecteur du centre vers le point actuel
    vec2 toPoint = pos - center;
    
    // Distance à l'obstacle
    float distToObstacle = length(toPoint) - radius;
    
    // Couleur par défaut - fond noir
    uint color = 0x000000AA;
    
    // Vérifier si le pixel est à l'intérieur de l'obstacle
    if (distToObstacle <= 0.0) {
        // Couleur de l'obstacle - gris foncé
        color = 0x555555AA;
    } else {
        // Vérifier si nous sommes dans la zone d'ombre de l'obstacle
        bool inShadow = false;
        
        // Si nous sommes à droite de l'obstacle
        if (toPoint.x > 0.0) {
            // Longueur totale de la zone d'ombre
            float shadowLength = radius * 3.0;
            
            // Distance horizontale depuis le bord droit de l'obstacle
            float shadowDistX = toPoint.x;
            
            if (shadowDistX < shadowLength) {
                // Progression normalisée le long de la zone d'ombre (0 au début, 1 à la fin)
                float t = shadowDistX / shadowLength;
                
                // Hauteur de la zone d'ombre, qui diminue progressivement et s'arrondit vers la fin
                float shadowHeight;
                
                if (t < 0.7) {
                    // Première partie: hauteur constante
                    shadowHeight = radius;
                } else {
                    // Dernière partie: réduction progressive avec courbe
                    // Utiliser une courbe pour créer un effet arrondi
                    float transition = (t - 0.7) / 0.3; // 0 au début de la transition, 1 à la fin
                    shadowHeight = radius * (1.0 - transition * transition);
                }
                
                // Vérifier si nous sommes dans la zone d'ombre à cette position
                if (abs(toPoint.y) < shadowHeight) {
                    inShadow = true;
                    // Dans la zone d'ombre, simplement noir (pas d'effet d'air)
                    color = 0x000000AA;
                }
            }
        }
        
        // Si nous ne sommes pas dans la zone d'ombre, flux d'air normal
        if (!inShadow) {
            // Créer des lignes horizontales denses pour l'écoulement d'air
            // Réduire la hauteur des lignes à 2 pixels (au lieu de 4)
            // et réduire l'espacement à 2.5 pixels (au lieu de 5)
            if (mod(float(y), 2.5) < 2.0) {
                // Position x modifiée par step pour animation
                float xAnimated = float(x) - float(step) * 1.5;
                
                // Déterminer si cette ligne est en contact avec l'obstacle
                // Vérifier si la ligne traverse le même niveau Y que l'obstacle
                bool lineHitsObstacle = (abs(pos.y - center.y) < radius);
                
                // Motif alterné pour rendre le mouvement visible
                if (mod(xAnimated, 20.0) < 10.0) {
                    if (lineHitsObstacle && pos.x > center.x) {
                        // Après contact avec l'obstacle - rouge
                        color = 0xFF0000AA;  // Rouge vif
                    } else {
                        // Bleu vif lumineux typique des logiciels de simulation
                        color = 0x3366FFAA;  // Bleu vif lumineux
                    }
                } else {
                    if (lineHitsObstacle && pos.x > center.x) {
                        // Après contact avec l'obstacle - rouge clair
                        color = 0xFF5555AA;  // Rouge clair
                    } else {
                        // Version plus claire du même bleu
                        color = 0x66AAFFAA;  // Bleu clair lumineux
                    }
                }
            }
        }
    }
    
    // Stocker la couleur
    data_0[p] = int(color);
}