// Adapte de https://www.shadertoy.com/view/wc2XRh
// Version optimisee pour la bibliotheque Compute Shader Studio dans Godot.
#define POINTCOUNT 8
#define PI 3.14159
#define iResolution vec2(float(WSX), float(WSY))
#define inBox(low, high, point) (low.x <= point.x && low.y <= point.y && high.x > point.x && high.y > point.y)
#define toUv(point) (2.0*(point - 0.5*iResolution)/iResolution.y)

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    vec2 fragCoord = vec2(x, y);
    vec2 uv = toUv(fragCoord);
    vec2 low = floor(uv);
    vec2 high = ceil(uv);
    uv = mod(uv, vec2(1.0));

    // t = "temps" bas sur step
    float t = float(step) * 0.006;

    vec2 points[POINTCOUNT];
    for (int i = 0; i < POINTCOUNT; i++) {
        float fi = float(i);
        points[i] = vec2(
            (1.75 - 0.125 * fi) * cos(1.333 * t - (1.0 - 0.2 * t) * fi / PI),
            (1.0 - 0.1 * fi) * sin((fi + 1.0) * t + PI * 0.5)
        );
    }

    int iters = 0;
    while (iters < 8) {
        iters++;
        bool sub = false;
        for (int i = 0; i < POINTCOUNT; i++) {
            if (inBox(low, high, points[i])) {
                sub = true;
                break;
            }
        }
        if (sub) {
            vec2 center = (high + low) * 0.5;
            if (uv.x > 0.5) {
                low.x = center.x;
                uv.x = uv.x * 2.0 - 1.0;
            } else {
                high.x = center.x;
                uv.x *= 2.0;
            }
            if (uv.y > 0.5) {
                low.y = center.y;
                uv.y = uv.y * 2.0 - 1.0;
            } else {
                high.y = center.y;
                uv.y *= 2.0;
            }
        } else {
            break;
        }
    }

    float m = max(abs(uv.x - 0.5), abs(uv.y - 0.5)) * 2.0;
    float s = smoothstep(1.0 - 0.01 * pow(2.0, float(iters)), 1.0, m);
    vec3 col = vec3(s);

    int a = 255;
    int r = int(clamp(col.r, 0.0, 1.0) * 255.0);
    int g = int(clamp(col.g, 0.0, 1.0) * 255.0);
    int b = int(clamp(col.b, 0.0, 1.0) * 255.0);

    data_0[p] = (a << 24) | (r <<8 ) | (g << 16) | b;
}
