extends Node

# Référence au ComputeShader
var compute_shader

# Structure pour les paramètres à passer au shader
var params = {
	"mouse_clicked": false,
	"mouse_position": Vector2(0, 0)
}

func _ready():
	# Récupérer le ComputeShader (ajustez le chemin selon votre structure)
	compute_shader = $/ProjetMaximeKERVRAN/ComputeShaderStudio2D
	
	# Initialiser les paramètres du shader
	update_shader_params()

func _input(event):
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
			# Mettre à jour les paramètres quand le bouton gauche est cliqué
			params.mouse_clicked = true
			params.mouse_position = event.position
			update_shader_params()
		elif event.button_index == MOUSE_BUTTON_LEFT and not event.pressed:
			# Réinitialiser si on relâche le bouton
			params.mouse_clicked = false
			update_shader_params()
	
	if event is InputEventMouseMotion and Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT):
		# Mettre à jour la position pendant que le bouton est enfoncé (optionnel)
		params.mouse_position = event.position
		update_shader_params()

func update_shader_params():
	# Passer les paramètres au shader
	compute_shader.set_shader_parameter("mouse_clicked", params.mouse_clicked)
	compute_shader.set_shader_parameter("mouse_position", params.mouse_position)
	
	# Déclencher le calcul du shader
	compute_shader.dispatch()
