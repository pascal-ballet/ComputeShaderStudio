extends Node2D
@onready var compute_shader = $ComputeShaderStudio2D
var attack_time = 0.0
var attack_progress = 0.0
var is_attacking = false
var current_attack = "none"

func _ready():
	# Initialiser les uniformes pour qu'ils soient disponibles dans le shader
	compute_shader.set_shader_parameter("params.TIME", 0.0)
	compute_shader.set_shader_parameter("params.PROGRESS", 0.0)
	compute_shader.set_shader_parameter("params.CENTER_X", 0.0)
	compute_shader.set_shader_parameter("params.CENTER_Y", 0.0)
	compute_shader.set_shader_parameter("params.TARGET_X", 0.0)
	compute_shader.set_shader_parameter("params.TARGET_Y", 0.0)
	compute_shader.set_shader_parameter("params.ATTACK_TYPE", 0) # 0=aucune, 1=vibrobscur, 2=lance-flammes

func _process(delta):
	if is_attacking:
		attack_time += delta
		attack_progress = min(attack_time / 2.0, 1.0)  # Animation de 2 secondes
		
		# Mettre à jour les uniformes
		compute_shader.set_shader_parameter("params.TIME", attack_time)
		compute_shader.set_shader_parameter("params.PROGRESS", attack_progress)
		
		# Fin de l'animation
		if attack_progress >= 1.0:
			is_attacking = false
			# Réinitialiser l'effet
			reset_attack()

func use_dark_pulse(source_x = 0, source_y = 0):
	current_attack = "dark_pulse"
	is_attacking = true
	attack_time = 0.0
	attack_progress = 0.0
	
	# Définir les paramètres pour Vibrobscur
	compute_shader.set_shader_parameter("params.CENTER_X", float(source_x))
	compute_shader.set_shader_parameter("params.CENTER_Y", float(source_y))
	compute_shader.set_shader_parameter("params.TIME", 0.0)
	compute_shader.set_shader_parameter("params.PROGRESS", 0.0)
	compute_shader.set_shader_parameter("params.ATTACK_TYPE", 1) # 1 = Vibrobscur

func use_flamethrower(source_x = 0, source_y = 0, target_x = 0, target_y = 0):
	current_attack = "flamethrower"
	is_attacking = true
	attack_time = 0.0
	attack_progress = 0.0
	
	# Définir les paramètres pour Lance-Flammes
	compute_shader.set_shader_parameter("params.CENTER_X", float(source_x))
	compute_shader.set_shader_parameter("params.CENTER_Y", float(source_y))
	compute_shader.set_shader_parameter("params.TARGET_X", float(target_x))
	compute_shader.set_shader_parameter("params.TARGET_Y", float(target_y))
	compute_shader.set_shader_parameter("params.TIME", 0.0)
	compute_shader.set_shader_parameter("params.PROGRESS", 0.0)
	compute_shader.set_shader_parameter("params.ATTACK_TYPE", 2) # 2 = Lance-Flammes

func reset_attack():
	# Réinitialiser à l'état sans attaque
	current_attack = "none"
	compute_shader.set_shader_parameter("params.ATTACK_TYPE", 0)
