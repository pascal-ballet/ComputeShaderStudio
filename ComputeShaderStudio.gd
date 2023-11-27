extends Node

# Global Workspace
@export var workspace_x:int = 128
@export var workspace_y:int = 128

@export_multiline var code:Array[String]

@export var data:Array[Sprite2D]

#@export var SZ:int = 1

var src_header = """
#[compute]
#version 450

// Invocations in the (x, y, z) dimension
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// A binding to the buffer we create in our script
layout(set = 0, binding = 0, std430) restrict buffer AllGrids {
	int data_1[];
	int data_2[];
};

uint WSX = 128;
	
"""
# Accessible variables
# uint x,y,z : from GlobalInvocationID (	uint x = gl_GlobalInvocationID.x;)
# uint GWSX,GWSY,GWSZ : from Global WorkSpace
# uint step : time step of the execution
# The Sprites that display the data
var src_main_1 = """
void main() {
	// Write your code here
	uint p = x + y * WSX;
	grids.data_1[p] += 1;
	grids.data_2[p] *= 2;
}
""" 











var rd 				: RenderingDevice
var shader 			: RID
var buffer 			: RID
var buffer_2 		: RID
var buffer_stage 	: RID
var uniform_stage 	: RDUniform
var pipeline		: RID
var uniform_set		: RID



var data_1:Sprite2D
var data_2:Sprite2D
var WSX:int = 128
var WSY:int = 128
# Called when the node enters the scene tree for the first time.
func _ready():
	WSX = workspace_x
	WSY = workspace_y
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
	var shader_file := load("res://script_1.glsl")
	var shader_spirv: RDShaderSPIRV = shader_file.get_spirv()
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

	# Buffer for stage
	var input_stage :PackedInt32Array = PackedInt32Array()
	for i in range(1):
		for j in range(1):
			input_stage.append(0)
	var input_stage_bytes := input_stage.to_byte_array()
	buffer_stage = rd.storage_buffer_create(input_stage_bytes.size(), input_stage_bytes)

	# *********************
	# *  UNIFORM CREATION *
	# *********************
	
	# Create uniform 0
	var uniform := RDUniform.new()
	uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform.binding = 0 # this needs to match the "binding" in our shader file
	uniform.add_id(buffer)

	# Create uniform 1
	var uniform_2 := RDUniform.new()
	uniform_2.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform_2.binding = 1 # this needs to match the "binding" in our shader file
	uniform_2.add_id(buffer_2)

	# Create Stage uniform 2
	uniform_stage = RDUniform.new()
	uniform_stage.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform_stage.binding = 2 # this needs to match the "binding" in our shader file
	uniform_stage.add_id(buffer_stage)
	
	# Create the uniform SET between CPU & GPU
	uniform_set = rd.uniform_set_create([uniform, uniform_2, uniform_stage], shader, 0) # the last parameter (the 0) needs to match the "set" in our shader file

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


var stage : int = 0
var step  : int = 0

func compute():
	var compute_list : int = rd.compute_list_begin() ######################
	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
	rd.compute_list_bind_uniform_set(compute_list, uniform_set, 0)
	rd.compute_list_dispatch(compute_list, WSX>>3, WSY>>3, 1)
	rd.compute_list_end() ##################################################

	# Submit to GPU and wait for sync
	rd.submit()
	rd.sync()
	
	display_all_values()
	
func _process(_delta):
	# compute()
	
	stage = (stage + 1) % 2
	#_update_stage()
	#print("step="+str(step))
	step += 1
	
	
	
	
	
	
#func _update_stage():
#	# Buffer for stage
#	var buffer_stage := rd.storage_buffer_create(4, PackedByteArray([stage]))
#
#	# Re-Create Stage uniform
#	uniform_stage = RDUniform.new()
#	uniform_stage.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
#	uniform_stage.binding = 2 # this needs to match the "binding" in our shader file
#	uniform_stage.add_id(buffer_stage)
#
#	var compute_list :  = rd.compute_list_begin()
#	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
#	rd.compute_list_bind_uniform_set(compute_list, rd.uniform_set_create([uniform_stage], shader, 0), 0)
#	rd.compute_list_dispatch(compute_list, 1, 1, 1)
#	rd.compute_list_end()
#
#	rd.submit()
#	rd.sync()



func _on_button_pressed():
	compute()
