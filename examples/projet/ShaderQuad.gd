extends TextureRect

@export var resolution: Vector2 = Vector2(640, 360)
var shader_material: ShaderMaterial
var time: float = 0.0

func _ready():
	# Crée et applique le matériel du shader
	shader_material = ShaderMaterial.new()
	shader_material.shader = preload("res://compute_shader.gdshader")
	self.material = shader_material
	self.stretch_mode = StretchMode.STRETCH_KEEP_ASPECT_CENTERED

	# Définir la taille du TextureRect à la résolution donnée
	self.rect_size = resolution  # Utilisez rect_size pour ajuster la taille du TextureRect

	# Définir les paramètres du shader au démarrage
	self.material.set_shader_param("resolution", resolution)

func _process(delta):
	# Incrémente le temps à chaque frame
	time += delta
	self.material.set_shader_param("time", time)  # Envoie le temps au shader
