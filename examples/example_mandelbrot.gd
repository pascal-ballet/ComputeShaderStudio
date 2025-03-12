extends Node

var css : Node
var zoom : float = 1.0
var pos : Vector2 = Vector2(0, 0)
var grab : Vector2
var grabbing : bool = false

func screen_to_world(screen : Vector2):
	var pos_data0 : Vector2 = css.screen_to_data0(screen)
	var wsize : Vector2 = Vector2(css.WSX, css.WSY);
	return pos + (pos_data0 / wsize - Vector2(0.5, 0.5)) * 4.0 / zoom;

func _ready():
	css = get_node("ComputeShaderStudio2D")
	# Augmenter la performance
	css.material.set_shader_parameter("num_iterations", 1)

func _process(_delta):
	# Pas besoin de passer des uniformes
	pass

func _input(event):
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_WHEEL_UP:
			zoom *= 1.1
		elif event.button_index == MOUSE_BUTTON_WHEEL_DOWN:
			zoom /= 1.1
		elif event.button_index == MOUSE_BUTTON_LEFT:
			if event.pressed:  # Start panning
				grab = screen_to_world(event.position)
				grabbing = true
			else:  # Stop panning
				grabbing = false
	
	elif event is InputEventMouseMotion and grabbing:  # Panning
		var world_pos = screen_to_world(event.position)
		var delta = world_pos - grab
		pos -= delta
