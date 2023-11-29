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
	uint p = x + y * WSX;
	
	if (step == 1 && current_pass == 0) {
		if (data_0[p] < 999990){
			data_0[p] = 0x0000FF00;
		} else {
			data_0[p] = 0x00FF0000;
		}
	}
	
	if (current_pass == 0) {
		//data_0[p] = data_0[p] / 2;
		data_1[p] = data_1[p] + 1024;
	}
	
}
""" 

























var GLSL_header = """
#[compute]
#version 450

// Invocations in the (x, y, z) dimension
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Bindings to the buffers we create in our script
layout(binding = 0) buffer Params {
	int step;
	int current_pass;
};

"""



@export var data:Array[Sprite2D]

var rd 				: RenderingDevice
var shader 			: RID
var buffers 		: Array[RID]
var buffer_params 	: RID

var uniforms		: Array[RDUniform]
#var uniform_2 		: RDUniform
var uniform_params 	: RDUniform

var bindings		: Array = []

var pipeline		: RID
var uniform_set		: RID

# Called when the node enters the scene tree for the first time.
func _ready():

	# Create a local rendering device.
	rd = RenderingServer.create_local_rendering_device()
	if not rd:
		set_process(false)
		print("Compute shaders are not available")
		return
		
	# *********************
	# *  SHADER CREATION  *
	# *********************

	var nb_buffers : int = data.size()

	# Create GLSL Header
	GLSL_header += """
uint WSX="""+str(WSX)+""";"""+"""
uint WSY="""+str(WSY)+""";
"""

	for i in nb_buffers:
		GLSL_header += """
layout(binding = """+str(i+1)+""") buffer Data"""+str(i)+""" {
	int data_"""+str(i)+"""[];
};

"""
	var GLSL_code : String = GLSL_header + GLSL_main
	print(GLSL_code)

	var shader_spirv: RDShaderSPIRV = string_to_file_to_spirv(GLSL_code)
	shader = rd.shader_create_from_spirv(shader_spirv)


	# *********************
	# *  BUFFERS CREATION *
	# *********************
	
	# Buffer for current_pass
	var input_params :PackedInt32Array = PackedInt32Array()
	input_params.append(step)
	input_params.append(current_pass)
	var input_params_bytes := input_params.to_byte_array()
	buffer_params = rd.storage_buffer_create(input_params_bytes.size(), input_params_bytes)
	
	# Buffers from/for data (Sprite2D)
	for b in nb_buffers:
		var input :PackedInt32Array = PackedInt32Array()
		for i in range(WSX):
			for j in range(WSY):
				input.append(randi())
		var input_bytes := input.to_byte_array()
		buffers.append(rd.storage_buffer_create(input_bytes.size(), input_bytes))

	# *********************
	# * UNIFORMS CREATION *
	# *********************
	
	# Create current_pass uniform pass
	uniform_params = RDUniform.new()
	uniform_params.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform_params.binding = 0 # this needs to match the "binding" in our shader file
	uniform_params.add_id(buffer_params)
	
	var nb_uniforms : int = data.size()
	for b in nb_uniforms:
		var uniform = RDUniform.new()
		uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
		uniform.binding = b+1 # this needs to match the "binding" in our shader file
		uniform.add_id(buffers[b])
		uniforms.append(uniform)

	# Create the uniform SET between CPU & GPU
	bindings = [uniform_params]
	for b in nb_buffers:
		bindings.append(uniforms[b])
	
	uniform_set = rd.uniform_set_create(bindings, shader, 0) # the last parameter (the 0) needs to match the "set" in our shader file

	# **************************
	# *  COMPUTE LIST CREATION *
	# **************************
	# Create a compute pipeline
	pipeline = rd.compute_pipeline_create(shader)
	
	# Execute once (should it stay?)
	compute()
	# Read back the data from the buffer
	display_all_values()

func display_all_values():
	# Read back the data from the buffer
	var output_bytes :   PackedByteArray = rd.buffer_get_data(buffers[0])
	display_values(data[0], output_bytes)
	# Read back the data from the buffer_2
	var output_bytes_2 : PackedByteArray = rd.buffer_get_data(buffers[1])
	display_values(data[1], output_bytes_2)

func display_values(sprite : Sprite2D, values : PackedByteArray): # PackedInt32Array):
	var image_format : int = Image.FORMAT_RGBA8
	var image := Image.create_from_data(128, 128, false, image_format, values)
	sprite.set_texture(ImageTexture.create_from_image(image))

var step  : int = 0

func compute():
	print("Step="+str(step)+" CurrentPass="+str(current_pass))
	
	_update_uniforms()
	
	# Prepare the Computer List ############################################
	var compute_list : int = rd.compute_list_begin()
	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
	rd.compute_list_bind_uniform_set(compute_list, uniform_set, 0)
	rd.compute_list_dispatch(compute_list, WSX>>3, WSY>>3, 1)
	rd.compute_list_end()
	#######################################################################

	# Submit to GPU and wait for sync
	rd.submit()
	rd.sync()
	
	# Update step and current_passe
	current_pass = (current_pass + 1) % nb_passes
	if current_pass == 0:
		step += 1
	

func _process(_delta):
	# compute()
	#display_all_values()
	pass
	

## Pass the intersting values from CPU to GPU
func _update_uniforms():
	# Buffer for current_pass
	var input_params :PackedInt32Array = PackedInt32Array()
	input_params.append(step)
	input_params.append(current_pass)
	var input_params_bytes := input_params.to_byte_array()
	buffer_params = rd.storage_buffer_create(input_params_bytes.size(), input_params_bytes)
	# Create current_pass uniform pass
	uniform_params = RDUniform.new()
	uniform_params.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform_params.binding = 0 # this needs to match the "binding" in our shader file
	uniform_params.add_id(buffer_params)
	bindings[0] = uniform_params
	
	uniform_set = rd.uniform_set_create(bindings, shader, 0)
	# Note: when changing the uniform set, use the same bindings Array (do not create a new Array)

func string_to_file_to_spirv(src:String)->RDShaderSPIRV:
	# This method should be changed but
	# I still don't know how to pass a String
	# instead of a Resource to compile the GLSL
	# (and the resource is loaded from the disc)
	# If you have a better solution, please give me :-)
	
	# Save str into a file
	var file_path = "res://my_shader.glsl" # chemin du fichier temporaire
	var file = FileAccess.open(file_path,FileAccess.WRITE)
	file.store_string(src)
	file.close()
	
	# A small delay for the file to close
	var start_time = Time.get_ticks_msec()
	var wait_duration = 1000 # Millisecondes
	while Time.get_ticks_msec() < start_time + wait_duration:
		pass # Bloc the execution to give time to the file to close

	# Load it as a resource GLSL shader
	var shader_file : Resource = load("res://my_shader.glsl")

	# Compile the glsl file
	var shader_spirv: RDShaderSPIRV = shader_file.get_spirv()
	
	# Remove the temp_file (finally, I prefere to leave the file: it can be useful)
	# if DirAccess.dir_exists_absolute(file_path):
	# 	DirAccess.remove_absolute(file_path)
	
	return shader_spirv

func _on_button_pressed():
	compute()
	display_all_values()
