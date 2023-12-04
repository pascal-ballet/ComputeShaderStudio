extends Node
class_name ComputeShaderStudio2D

var current_pass 	: int = 0

# Put your GLSL code in the GLSL_main string below
# Here are all the accessible variables (uniforms) inside your GLSL code:
#   uint x,y     : from GlobalInvocationID.x and .y
#   uint p       : the position [x][y] in the invocation
#   uint WSX,WSY : the Global WorkSpace of invocations (generally match the data size)
#   int* data_0, data_1, etc : are the data treated (can be displayed by Sprite2D).
#                  Access them by data_0[p], data_1[p], etc
#   uint step    : simulation step of the execution. Incresed by 1 after nb_passes
#   uint nb_passes: the number of passes your code needs (by step).
# 					There is a barrier between each pass.
#   uint current_pass: pass currently executed (one pass per frame, nb_passes by step)

#region ComputeShaderStudio

var GLSL_header = """
#version 450

// Invocations in the (x, y, z) dimension
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Bindings to the buffers we create in our script
layout(binding = 0) buffer Params {
	int step;
	int current_pass;
};

"""
## Print the current step.
@export var print_step:bool = false
## Print the current pass.
@export var print_passes:bool = false
## Print in Output all the generated code.
## Can be usefull for debugging.
@export var print_generated_code:bool = false
## Do not execute compute shader at launch.
@export var pause:bool = false
## Number of passes (synchronized code) needed.
@export var nb_passes		: int = 1
## Workspace Size X, usually it matches the x size of your Sprite2D image
@export var WSX				: int = 128
## Workspace Size Y, usually it matches the y size of your Sprite2D image
@export var WSY				: int = 128

## Write your GLSL code here
@export_multiline var GLSL_code : String = """
// Write your code HERE
void main() {
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint p = x + y * WSX;
	data_0[p] = 0xFFF00FFF - int(p)*(step+1);
	data_1[p] = 0xFF0000AA + int( 1.0 + 99999.9*sin(float(x+float(step+y))/1000.0));
}
"""
## Drag and drop your Sprite2D here.
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
#region _ready
func _ready():
	compile()

func compile():
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
	var GLSL_all : String = GLSL_header + GLSL_code
	if print_generated_code == true:
		print(GLSL_all)
	
	# Compile the shader by passing a string
	var shader_src := RDShaderSource.new()
	shader_src.set_stage_source(RenderingDevice.SHADER_STAGE_COMPUTE, GLSL_all)
	var shader_spirv := rd.shader_compile_spirv_from_source(shader_src)
	
	var err:String=shader_spirv.compile_error_compute
	
	if err != "":
		printerr(err)
		get_tree().quit()
	
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
		var input_bytes :PackedByteArray = input.to_byte_array()
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
	# compute()
	# Read back the data from the buffer
	# display_all_values()
#endregion

func display_all_values():
	# Read back the data from the buffers
	for b in data.size():
		var output_bytes :   PackedByteArray = rd.buffer_get_data(buffers[b])
		if is_instance_valid(data[b]):
			display_values(data[b], output_bytes)

func display_values(sprite : Sprite2D, values : PackedByteArray): # PackedInt32Array):
	var image_format : int = Image.FORMAT_RGBA8
	var image := Image.create_from_data(WSX, WSY, false, image_format, values)
	sprite.set_texture(ImageTexture.create_from_image(image))

var step  : int = 0

func compute():
	if print_step == true && current_pass%nb_passes == 0:
		print("Step="+str(step))
	if print_passes == true:
		print(" CurrentPass="+str(current_pass))
	
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
	if pause == false:
		compute()
		display_all_values()

## Pass the interesting values from CPU to GPU
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

#endregion

func _on_button_step():
	pause = true
	compute()
	display_all_values()


func _on_button_play():
	pause = false # Replace with function body.
