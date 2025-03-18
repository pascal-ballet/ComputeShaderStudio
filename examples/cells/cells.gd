# NOTE: modifier from ComputeShaderStudio

extends Node

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

var GLSL_header = """
#version 450

// Invocations in the (x, y, z) dimension
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Bindings to the buffers we create in our script
// layout(binding = 0) buffer Params
layout(push_constant) uniform Params {
	// NOTE: push_constant uniform block size aligned 16
	int current_step;
	int current_pass;
	int click_x;
	int click_y;
	int display_mode;
};
"""

## Print the current step.
@export var print_step:bool = false
## Print in Output all the generated code.
## Can be usefull for debugging.
@export var print_generated_code:bool = false
## Do not execute compute shader at launch.
@export var pause : bool = false
## Number of passes (synchronized code) needed.
var nb_passes		: int = 1
## Workspace Size X, usually it matches the x size of your Sprite2D image
@export var WSX				: int = 128
## Workspace Size Y, usually it matches the y size of your Sprite2D image
@export var WSY				: int = 128

@export var nb_passes_cyto : int = 10
@export var nb_passes_memb : int = 10

@export_file("*")
var glsl_file : String = ""

## Drag and drop your Sprite2D here.
@export var data : Array[Sprite2D]

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
const uint WSX="""+str(WSX)+""";"""+"""
const uint WSY="""+str(WSY)+""";
"""

	var pass_acc := 0

	GLSL_header += "const int PASS_INIT = " + str(pass_acc) + ";\n"
	GLSL_header += "const int PASS_CYTO_INIT = " + str(pass_acc + 1) + ";\n"
	pass_acc += 1 + nb_passes_cyto * 2
	GLSL_header += "const int PASS_CYTO_FINI = " + str(pass_acc - 1) + ";\n"
	GLSL_header += "const int PASS_MEMB = " + str(pass_acc) + ";\n"
	GLSL_header += "const int PASS_MEMB_INIT = " + str(pass_acc + 1) + ";\n"
	pass_acc += 1 + nb_passes_memb * 2
	GLSL_header += "const int PASS_MEMB_FINI = " + str(pass_acc - 1) + ";\n"

	GLSL_header += "const int PASS_DIRECTION = " + str(pass_acc) + ";\n"
	GLSL_header += "const int PASS_MOVE = " + str(pass_acc + 1) + ";\n"
	GLSL_header += "const int PASS_MOVE_2 = " + str(pass_acc + 2) + ";\n"
	GLSL_header += "const int PASS_MOVE_3 = " + str(pass_acc + 3) + ";\n"
	pass_acc = pass_acc + 4

	GLSL_header += "const int PASS_LAST = " + str(pass_acc) + ";\n"
	nb_passes = pass_acc + 1

	GLSL_header += "const int PASS_COUNT = " + str(nb_passes) + ";\n"
	for i in nb_buffers:
		GLSL_header += """
layout(binding = """+str(i)+""") buffer Data"""+str(i)+""" {
	int data_"""+str(i)+"""[];
};

"""

	var GLSL_all : String

	if !glsl_file.is_empty():
		var file := FileAccess.open(glsl_file, FileAccess.READ)
		GLSL_all = GLSL_header + file.get_as_text()

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

	var nb_uniforms : int = data.size()

	for b in nb_uniforms:
		var uniform := RDUniform.new()
		uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
		uniform.binding = b # this needs to match the "binding" in our shader file
		uniform.add_id(buffers[b])
		uniforms.append(uniform)

	# Create the uniform SET between CPU & GPU
	bindings = []

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
	# for b in data.size():

	for b in range(len(buffers)):
		if is_instance_valid(data[b]):
			var output_bytes : PackedByteArray = rd.buffer_get_data(buffers[b])
			display_values(data[b], output_bytes)

func display_values(sprite : Sprite2D, values : PackedByteArray): # PackedInt32Array):
	var image_format : int = Image.FORMAT_RGBA8
	var image := Image.create_from_data(WSX, WSY, false, image_format, values)
	sprite.set_texture(ImageTexture.create_from_image(image))

var current_step : int = 0

func compute():
	if print_step:
		print("Step=" + str(current_step))

	# Prepare the Computer List ############################################
	var compute_list := rd.compute_list_begin()

	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
	rd.compute_list_bind_uniform_set(compute_list, uniform_set, 0)

	for i in nb_passes:
		var push_constants := _pack_params(i)
		rd.compute_list_set_push_constant(compute_list, push_constants, len(push_constants))
		rd.compute_list_dispatch(compute_list, WSX >> 3, WSY >> 3, 1)
		rd.compute_list_add_barrier(compute_list) # sync

	rd.compute_list_end()
	#######################################################################

	current_step += 1

func _process(_delta):
	if pause == false:
		var start := Time.get_ticks_usec()

		compute()

		var mid := Time.get_ticks_usec()

		# Submit to GPU and wait for sync
		rd.submit()
		rd.sync()

		var end := Time.get_ticks_usec()

		display_all_values()

		var post := Time.get_ticks_usec()

		print("prepare time: " + str(float(mid - start) / 1000.0) + "ms")
		print("compute time: " + str(float(end - mid) / 1000.0) + "ms")
		print("present time: " + str(float(post - end) / 1000.0) + "ms")

# HACK: pass clicks

# this is used to 
var was_click := false

var display_mode := 0

func _pack_params(which_pass : int) -> PackedByteArray:
	# HACK
	# TODO move this out of here and do this only once
	var click_pos := data[0].get_local_mouse_position() + (Vector2(WSX, WSY) / 2)
	var is_click := Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT)
	var click_pos_clamp : Vector2 = clamp(click_pos, Vector2(0, 0), Vector2(WSX, WSY))

	if is_click and not was_click and click_pos == click_pos_clamp:
		click_pos = click_pos_clamp

	else:
		click_pos = Vector2(-1, -1)

	was_click = is_click

	var input_params : PackedInt32Array = PackedInt32Array([current_step, which_pass,
		int(click_pos.x), int(click_pos.y), display_mode, 0, 0, 0])
	return input_params.to_byte_array()

func set_display_normal():
	display_mode = 0

func set_display_cytoplasm():
	display_mode = 1

func set_display_cytoplasm_flat():
	display_mode = 2

func set_display_membranes():
	display_mode = 3

func set_display_membrane_field():
	display_mode = 4
