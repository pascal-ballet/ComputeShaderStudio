extends Node2D

@export var particle_count: int = 10000
var particles: ImageTexture
var shader_material: ShaderMaterial

func _ready():
    create_particles()
    shader_material = ShaderMaterial.new()
    shader_material.shader = preload("res://compute_shader.gdshader")
    $Particles.material = shader_material

func create_particles():
    var img = Image.create(particle_count, 1, false, Image.FORMAT_RGBAF)
    img.lock()
    for i in range(particle_count):
        img.set_pixel(i, 0, Color(randf(), randf(), 0, 1))
    img.unlock()
    particles = ImageTexture.create_from_image(img)
    $Particles.texture = particles