/* MANDELBROT 
	Josselin SCOUARNEC
	Feb 2024
    Adapted from https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set
*/

#define MAX_IT 500
#define PI 3.141592
#define TWO_PI (2.0*3.141592)

#define RADIUS 2.0

layout(binding = 1) buffer UserUniforms {
    float u_zoom;
    float u_pos_x;
    float u_pos_y;
};

#define PALETTE_SIZE 16
const vec3 palette[PALETTE_SIZE] = {
    vec3(0.0, 0.5, 0.5),
    vec3(0.0, 0.0, 0.5),
    vec3(0.0, 0.0, 1.0),
    vec3(0.0, 0.5, 1.0),
    vec3(0.0, 1.0, 1.0),
    vec3(0.5, 1.0, 0.5),
    vec3(1.0, 1.0, 0.0),
    vec3(1.0, 0.5, 0.0),
    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 0.5, 0.0),
    vec3(1.0, 1.0, 0.0),
    vec3(0.5, 1.0, 0.5),
    vec3(0.0, 1.0, 1.0),
    vec3(0.0, 0.5, 1.0),
    vec3(0.0, 0.0, 1.0),
    vec3(0.0, 0.0, 0.5),
};

int rgb_to_int(vec3 c) {
    return 0xFF000000
        | int(255.0*c.x)
        | int(255.0*c.y) << 8
        | int(255.0*c.z) << 16;
}

vec3 coloring(vec2 z, uint it) {
    if (it >= MAX_IT) {
        return vec3(0.0);
    }

    // Smoothing
    float logz = log(z.x*z.x + z.y*z.y) / 2;
    float nu = log(logz / log(2)) / log(2);
    float smooth_iter = it + 1.0 - nu;

    // Index in palette
    float index = 3 * log(smooth_iter) * PALETTE_SIZE / log(MAX_IT);

    // Linear interpolation
    vec3 c1 = palette[int(index)];
    vec3 c2 = palette[(int(index) + 1) % PALETTE_SIZE];
    return mix(c1, c2, fract(index));
}

vec3 fractal(vec2 z, vec2 c) {
    uint it;
    for (it = 0; it < MAX_IT && z.x*z.x + z.y*z.y < RADIUS*RADIUS; it++) {
        // z := z*z + c     (a+bi) * (a+bi) = a*a - b*b + 2abi
        z = vec2(z.x*z.x - z.y*z.y + c.x, 2*z.x*z.y + c.y);
    }
    return coloring(z, it);
}

vec2 screen_to_world(vec2 screen) {
    vec2 wsize  = vec2(WSX, WSY);
    vec2 center = vec2(u_pos_x, u_pos_y);
    return center + (screen / wsize - 0.5) * 4.0 / u_zoom;
}

void main() {
    uint p = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * WSX;

    // Julia
    vec2 wsize  = vec2(WSX, WSY);
    vec2 z = (gl_GlobalInvocationID.xy / wsize - 0.5) * 4.0;
    vec2 c = screen_to_world(vec2(mousex, mousey));
    vec3 color = fractal(z, c);
    data_1[p] = rgb_to_int(color);

    // Cursor
    if ((mousex >= 0 && gl_GlobalInvocationID.x == mousex)
     || (mousey >= 0 && gl_GlobalInvocationID.y == mousey))
    {
        data_0[p] = rgb_to_int(vec3(1.0, 0.0, 0.0));
    }
    else { // Mandelbrot
        vec2 z = screen_to_world(gl_GlobalInvocationID.xy);
        vec2 c = z;
        vec3 color = fractal(z, c);
        data_0[p] = rgb_to_int(color);
    }
}
