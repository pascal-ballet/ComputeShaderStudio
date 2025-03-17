extends Label


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	var step : int = $"../ComputeShaderStudio2D2".step
	var current_pass : int = $"../ComputeShaderStudio2D2".current_pass
	self.text = "step="+str(step)+"    current_pass="+str(current_pass)
