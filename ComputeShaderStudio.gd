extends Node


@export var data_1:Sprite2D
@export var data_2:Sprite2D

# Global Workspace
@export var GWSX:int = 128
@export var GWSY:int = 128
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
}
grids;

uint GWSX = 128;
	
"""
# Accessible variables
# uint x,y,z : from GlobalInvocationID (	uint x = gl_GlobalInvocationID.x;)
# uint GWSX,GWSY,GWSZ : from Global WorkSpace
# uint step : time step of the execution
# The Sprites that display the data
var src_main = """
void main() {
	// Write your code here
	uint p = x + y * GWSX;
	grids.data_1[p] += 1;
	//grids.data_2[p] *= 2;
}
""" 











var rd : RenderingDevice
# Called when the node enters the scene tree for the first time.
func _ready():
	# Create a local rendering device.
	rd = RenderingServer.create_local_rendering_device()

	# Load GLSL shader
	var shader_file := load("res://script_1.glsl")
	var shader_spirv: RDShaderSPIRV = shader_file.get_spirv()
	var shader := rd.shader_create_from_spirv(shader_spirv)

	# Prepare our data. We use ints in the shader, so we need 32 bit.
	var input :PackedInt32Array = PackedInt32Array()
	for i in range(GWSX):
		for j in range(GWSY):
			input.append(randi())
	var input_bytes := input.to_byte_array()

	# Create a storage buffer that can hold our float values.
	# Each float has 4 bytes (32 bit) so 10 x 4 = 40 bytes
	var buffer := rd.storage_buffer_create(input_bytes.size(), input_bytes)

	# Create a uniform to assign the buffer to the rendering device
	var uniform := RDUniform.new()

	uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform.binding = 0 # this needs to match the "binding" in our shader file
	uniform.add_id(buffer)
	var uniform_set := rd.uniform_set_create([uniform], shader, 0) # the last parameter (the 0) needs to match the "set" in our shader file

	# Create a compute pipeline
	var pipeline : RID = rd.compute_pipeline_create(shader)
	var compute_list : int = rd.compute_list_begin()
	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
	rd.compute_list_bind_uniform_set(compute_list, uniform_set, 0)
	rd.compute_list_dispatch(compute_list, GWSX>>3, GWSY>>3, 1)
	rd.compute_list_end()


	# Submit to GPU and wait for sync
	rd.submit()
	rd.sync()


	# Read back the data from the buffer
	var output_bytes := rd.buffer_get_data(buffer)
	var output := output_bytes.to_int32_array()
	display_values(output)

func display_values(values : PackedInt32Array):
	var img:Image = Image.create(GWSX,GWSY,false, Image.FORMAT_RGBA8)
	for i in range(GWSX):
		for j in range(GWSY):
			var p:int = i+j*GWSX
			var v:int = values[p]
			var r:float = ((v & 0x0000FF00) >> 8)/255.0
			var g:float = ((v & 0x00FF0000) >> 16)/255.0
			var b:float = ((v & 0xFF000000) >> 24)/255.0
			img.set_pixel(i,j,Color(r,g,b,1.0) )
	data_1.set_texture(ImageTexture.create_from_image(img))

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
#	# Submit to GPU and wait for sync
	rd.submit()
	rd.sync()
	pass

