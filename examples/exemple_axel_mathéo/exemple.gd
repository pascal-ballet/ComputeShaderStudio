extends Node

var css : Node  # Référence au nœud Compute Shader Studio

func _ready():
    css = get_node("ComputeShaderStudio2D")  
    # Configuration initiale si nécessaire

func _process(_delta):
    # Logique de mise à jour du shader si requise
    pass