extends Node2D


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass


func _on_button_pressed() -> void:
	$ComputeShaderStudio2D2.pause = !$ComputeShaderStudio2D2.pause
	if $ComputeShaderStudio2D2.pause:
		$Button.text = "Play"
	else:
		$Button.text = "Pause" # Replace with function body.
