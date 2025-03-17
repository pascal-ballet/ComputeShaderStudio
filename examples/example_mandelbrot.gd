extends Node

var css : Node

var zoom : float = 1.0
var pos : Vector2 = Vector2(-0.75, 0)

var grab : Vector2
var grabbing : bool = false

func screen_to_world(screen : Vector2):
	var pos_data0 : Vector2 = css.screen_to_data0(screen)
	var wsize : Vector2 = Vector2(css.WSX, css.WSY);
	return pos + (pos_data0 / wsize - Vector2(0.5, 0.5)) * 4.0 / zoom;

func _ready():
	css = get_node("ComputeShaderStudio2D")
	css.uniform_user_data.resize(4*3)

func _process(_delta):
	css.uniform_user_data.encode_float(0, zoom)
	css.uniform_user_data.encode_float(4, pos.x)
	css.uniform_user_data.encode_float(8, pos.y)

func _input(event):
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_WHEEL_UP :
			zoom *= 1.1
		elif event.button_index == MOUSE_BUTTON_WHEEL_DOWN :
			zoom /= 1.1
		elif event.button_index == MOUSE_BUTTON_LEFT :
			if event.pressed : # Start panning
				grab = screen_to_world(event.position)
				grabbing = true
			else : # Stop panning
				grabbing = false
	
	elif event is InputEventMouse and grabbing : # Panning
		var delta : Vector2 = screen_to_world(event.position) - grab
		pos -= delta
