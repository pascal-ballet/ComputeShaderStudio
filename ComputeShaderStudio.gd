extends Node

var WSX				: int = 128 # Workspace Size X, usually it matches the x size of your Sprite2D
var WSY				: int = 128 # Workspace Size Y, usually it matches the y size of your Sprite2D
var nb_passes		: int = 2
var current_pass 	: int = 0

# Put your GLSL code in the GLSL_main string below
# Here are all the accessible variables (uniforms) inside your GLSL code:
#   uint x,y     : from GlobalInvocationID.x and .y
#   uint p       : the position [x][y] in the invocation
#   uint WSX,WSY : the Global WorkSpace of invocations (generally match the data size)
#   int* data_0, data_1, etc : are the data displayed by your Sprite2D.
#                  Access them by data_0[p], data_1[p], etc
#   uint step    : time step of the execution. Incresed by 1 after nb_passes
#  uint nb_passes: the number of passes your code needs to work
#  uint current_pass: which pass is currently executed (one pass per frame)
var GLSL_main = """
// Write your code HERE
void main() {
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint p = x + y * 128;
	
	if (current_pass == 0) {
		data_0[p] = data_0[p] / 2;
		data_1[p] = data_1[p] + 1024;
	}
	
	//if (current_pass == 0)
	//    data_1[p] += 10;
	//if (current_pass == 1)
	//    data_1[p] -= 10;
	
}
""" 

























var GLSL_header = """
#[compute]
#version 450

// Invocations in the (x, y, z) dimension
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// A binding to the buffer we create in our script
layout(binding = 0) buffer Data0 {
	int data_0[];
};

layout(binding = 1) buffer Data1 {
	int data_1[];
};

layout(binding = 2) buffer Params {
	int current_pass;
};

	
"""

@export var data:Array[Sprite2D]

var rd 				: RenderingDevice
var shader 			: RID
var buffer 			: RID
var buffer_2 		: RID
var buffer_pass 	: RID


var uniform 		: RDUniform
var uniform_2 		: RDUniform
var uniform_pass 	: RDUniform


var pipeline		: RID
var uniform_set		: RID



var data_1:Sprite2D
var data_2:Sprite2D

# Called when the node enters the scene tree for the first time.
func _ready():
	data_1 = data[0]
	data_2 = data[1]
	# Create a local rendering device.
	rd = RenderingServer.create_local_rendering_device()
	if not rd:
		set_process(false)
		print("Compute shaders are not available")
		return
		
	# *********************
	# *  SHADERS CREATION *
	# *********************
	# Load GLSL shader
	# var shader_file : Resource = load("res://script_1.glsl")
	#var shader_spirv: RDShaderSPIRV = shader_file.get_spirv()

	var shader_spirv: RDShaderSPIRV = string_to_file_to_spirv(GLSL_header + GLSL_main)
	shader = rd.shader_create_from_spirv(shader_spirv)



	# *********************
	# *  BUFFERS CREATION *
	# *********************
	
	# Buffer 1
	var input :PackedInt32Array = PackedInt32Array()
	for i in range(WSX):
		for j in range(WSY):
			input.append(randi())
	var input_bytes := input.to_byte_array()
	buffer = rd.storage_buffer_create(input_bytes.size(), input_bytes)

	# Buffer 2
	var input_2 :PackedInt32Array = PackedInt32Array()
	for i in range(WSX):
		for j in range(WSY):
			input_2.append(randi()%65536)
	var input_bytes_2 := input_2.to_byte_array()
	buffer_2 = rd.storage_buffer_create(input_bytes_2.size(), input_bytes_2)

	# Buffer for current_pass
	var input_pass :PackedInt32Array = PackedInt32Array()
	for i in range(1):
		for j in range(1):
			input_pass.append(0)
	var input_pass_bytes := input_pass.to_byte_array()
	buffer_pass = rd.storage_buffer_create(input_pass_bytes.size(), input_pass_bytes)

	# *********************
	# *  UNIFORM CREATION *
	# *********************
	
	# Create uniform
	uniform = RDUniform.new()
	uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform.binding = 0 # this needs to match the "binding" in our shader file
	uniform.add_id(buffer)

	# Create uniform 2
	uniform_2 = RDUniform.new()
	uniform_2.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform_2.binding = 1 # this needs to match the "binding" in our shader file
	uniform_2.add_id(buffer_2)

	# Create current_pass uniform pass
	uniform_pass = RDUniform.new()
	uniform_pass.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform_pass.binding = 2 # this needs to match the "binding" in our shader file
	uniform_pass.add_id(buffer_pass)
	
	# Create the uniform SET between CPU & GPU
	uniform_set = rd.uniform_set_create([uniform, uniform_2, uniform_pass], shader, 0) # the last parameter (the 0) needs to match the "set" in our shader file

	# **************************
	# *  COMPUTE LIST CREATION *
	# **************************
	# Create a compute pipeline
	pipeline = rd.compute_pipeline_create(shader)
	
	compute()
	# Read back the data from the buffer
	display_all_values()

func display_all_values():
	# Read back the data from the buffer
	var output_bytes :   PackedByteArray = rd.buffer_get_data(buffer)
	display_values(data_1, output_bytes)
	# Read back the data from the buffer_2
	var output_bytes_2 : PackedByteArray = rd.buffer_get_data(buffer_2)
	display_values(data_2, output_bytes_2)

func display_values(sprite : Sprite2D, values : PackedByteArray): # PackedInt32Array):
	var image_format : int = Image.FORMAT_RGBA8
	var image := Image.create_from_data(128, 128, false, image_format, values)
	sprite.set_texture(ImageTexture.create_from_image(image))



var step  : int = 0

func compute():
	print("Step="+str(step)+" CurrentPass="+str(current_pass))
	
	
	# Update the uniform
#
#	# ReCreate current_pass uniform pass
#	uniform_pass = RDUniform.new()
#	uniform_pass.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
#	uniform_pass.binding = 2 # this needs to match the "binding" in our shader file
#	uniform_pass.add_id(buffer_pass)
	
	
	# Create the uniform SET between CPU & GPU
	if step == 2:
		rd.global_shader_parameter_set('current_pass', current_pass)
#		buffer_pass = rd.storage_buffer_create(4, PackedByteArray([current_pass]))
#		uniform_pass.clear_ids()
#		uniform_pass.add_id(buffer_pass)
#		uniform_set = rd.uniform_set_create([uniform, uniform_2, uniform_pass], shader, 0) # the last parameter (the 0) needs to match the "set" in our shader file
#
	
	#######################################################################
	var compute_list : int = rd.compute_list_begin() ######################
	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
	
	rd.compute_list_bind_uniform_set(compute_list, uniform_set, 0)
	
	
	rd.compute_list_dispatch(compute_list, WSX>>3, WSY>>3, 1)
	rd.compute_list_end() ##################################################
	#######################################################################



	# Submit to GPU and wait for sync
	rd.submit()
	rd.sync()
	
	# Update step and passes
	
	current_pass = (current_pass + 1) % nb_passes
	#_update_current_pass()
	#print("step="+str(step))
	if current_pass == 0:
		step += 1
	

func _process(_delta):
	# compute()
	

	
	#display_all_values()
	
	pass
	
#var buffer_pass : RID
func _update_current_pass():
	# Buffer for current pass
	buffer_pass = rd.storage_buffer_create(4, PackedByteArray([current_pass]))

	# Re-Create current_pass uniform
	uniform_pass = RDUniform.new()
	uniform_pass.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform_pass.binding = 2 # this needs to match the "binding" in our shader file
	uniform_pass.add_id(buffer_pass)

	var compute_list :  = rd.compute_list_begin()
	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
	rd.compute_list_bind_uniform_set(compute_list, rd.uniform_set_create([uniform_pass], shader, 0), 0)
	rd.compute_list_dispatch(compute_list, 1, 1, 1)
	rd.compute_list_end()

	rd.submit()
	rd.sync()

func string_to_file_to_spirv(src:String)->RDShaderSPIRV:
	# Save str into a file
	var file_path = "res://my_shader.glsl" # chemin du fichier temporaire
	var file = FileAccess.open(file_path,FileAccess.WRITE)
	file.store_string(src)
	file.close()
	# Load it as a resource GLSL shader
	var shader_file : Resource = load("res://my_shader.glsl")

	# Compile the glsl file
	var shader_spirv: RDShaderSPIRV = shader_file.get_spirv()
	
	# Remove the temp_file
	# if DirAccess.dir_exists_absolute(file_path):
	# 	DirAccess.remove_absolute(file_path)
	
	return shader_spirv

func _on_button_pressed():
	compute()
	display_all_values()
