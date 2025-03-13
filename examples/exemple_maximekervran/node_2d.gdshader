shader_type canvas_item;

void fragment() {
    // Centrer les UV (elles vont de 0 à 1 → on les centre à 0)
    vec2 uv = UV - vec2(0.5);
    
    // Calcul de la distance au centre
    float dist = length(uv);

    // Rayon du cercle : 0.5 = tout le Sprite
    if (dist > 0.5) {
        discard; // On efface tout ce qui est hors du cercle
    }

    // Couleur jaune dorée
    COLOR = vec4(1.0, 0.84, 0.0, 1.0); // or
}

//void light() {
	// Called for every pixel for every light affecting the CanvasItem.
	// Uncomment to replace the default light processing function with this one.
//}
