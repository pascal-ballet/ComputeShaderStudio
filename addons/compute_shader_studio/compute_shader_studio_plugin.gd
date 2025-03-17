## BehaviorTreeForGroups
## ******************
## Editor plugin configuration

@tool
extends EditorPlugin

func _enter_tree():
	# Initialization of the plugin goes here.
	# Main Node
	add_custom_type("ComputeShaderStudio2D", "Node",\
			preload("compute_shader_studio_2d.gd"),preload("icon.png"))

func _exit_tree():
	# Clean-up of the plugin goes here.
	# Main Node
	remove_custom_type("ComputeShaderStudio2D")
