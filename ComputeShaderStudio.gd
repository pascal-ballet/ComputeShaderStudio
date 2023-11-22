extends Node

var SX:int = 128
var SY:int = 128

# Called when the node enters the scene tree for the first time.
func _ready():
	# Create a local rendering device.
	var rd := RenderingServer.create_local_rendering_device()

	# Load GLSL shader
	var shader_file_1 := load("res://script_1.glsl")
	var shader_file_2 := load("res://script_1.glsl")
	var shader_spirv_1: RDShaderSPIRV = shader_file_1.get_spirv()
	var shader_spirv_2: RDShaderSPIRV = shader_file_2.get_spirv()
	var shader_1 := rd.shader_create_from_spirv(shader_spirv_1)
	var shader_2 := rd.shader_create_from_spirv(shader_spirv_2)

	# Prepare our data. We use ints in the shader, so we need 32 bit.
	var input_1 :PackedInt32Array = PackedInt32Array()
	for i in range(SX):
		for j in range(SY):
			input_1.append(randi())
	var input_bytes_1 := input_1.to_byte_array()

	var input_2 :PackedInt32Array = PackedInt32Array()
	for i in range(SX):
		for j in range(SY):
			input_2.append(randi())
	var input_bytes_2 := input_2.to_byte_array()

	# Create a storage buffer that can hold our float values.
	# Each float has 4 bytes (32 bit) so 10 x 4 = 40 bytes
	var buffer_1 := rd.storage_buffer_create(input_bytes_1.size(), input_bytes_1)
	var buffer_2 := rd.storage_buffer_create(input_bytes_2.size(), input_bytes_2)

	# Create a uniform to assign the buffer to the rendering device
	var uniform_1 := RDUniform.new()
	var uniform_2 := RDUniform.new()

	uniform_1.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform_1.binding = 0 # this needs to match the "binding" in our shader file
	uniform_1.add_id(buffer_1)
	var uniform_set_1 := rd.uniform_set_create([uniform_1], shader_1, 0) # the last parameter (the 0) needs to match the "set" in our shader file

	uniform_2.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform_2.binding = 0 # this needs to match the "binding" in our shader file
	uniform_2.add_id(buffer_2)
	var uniform_set_2 := rd.uniform_set_create([uniform_2], shader_2, 1) # the last parameter (the 1) needs to match the "set" in our shader file

	# Create a compute pipeline
	var pipeline := rd.compute_pipeline_create(shader_1)
	var compute_list := rd.compute_list_begin()
	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
	rd.compute_list_bind_uniform_set(compute_list, uniform_set, 0)
	rd.compute_list_dispatch(compute_list, SX>>3, SY>>3, 1)
	rd.compute_list_end()

	# Submit to GPU and wait for sync
	rd.submit()
	rd.sync()

	# Read back the data from the buffer
	var output_bytes := rd.buffer_get_data(buffer)
	var output := output_bytes.to_int32_array()
	display_values(output)

func display_values(values : PackedInt32Array):
	var img:Image = Image.create(SX,SY,false, Image.FORMAT_RGBA8)
	for i in range(SX):
		for j in range(SY):
			var p:int = i+j*SX
			var v:int = values[p]
			var r:float = ((v & 0x0000FF00) >> 8)/255.0
			var g:float = ((v & 0x00FF0000) >> 16)/255.0
			var b:float = ((v & 0xFF000000) >> 24)/255.0
			img.set_pixel(i,j,Color(r,g,b,1.0) )
	$ComputeShaderMatrices/Matrix1.set_texture(ImageTexture.create_from_image(img))

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	pass

