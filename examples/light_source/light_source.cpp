// Enhanced artistic shader - Prismatic Flow
// Inspired by Danilo Guanabara with artistic extensions



//AUTHORS: NASSIM & YASSER
void main()
{
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int idx = x + y * int(WSX);
    // Normalized coordinates
    vec2 fragCoord = vec2(float(x), float(y));
    vec2 resolution = vec2(float(WSX), float(WSY));
    float time = float(step) * 0.03;

    // Mouse interaction
    vec2 mouse = vec2(float(mousex), float(mousey)) / resolution;

    // Calculate artistic effect
    vec3 c = vec3(0.0);
    float l = 0.0;
    float z = time;

    // Create multiple artistic layers with different parameters
    for (int i = 0; i < 3; i++)
    {
        vec2 uv, p;

        // Normalized coordinates (0 to 1)
        p = fragCoord / resolution;
        uv = p;

        // Center coordinates (-0.5 to 0.5)
        p -= 0.5;

        // Fix aspect ratio
        p.x *= resolution.x / resolution.y;

        // Increment z for each color channel with variation
        z += 0.07 + sin(time * 0.2) * 0.02;

        // Calculate length for distortion
        l = length(p);

        // Apply artistic distortion - enhanced with variations
        float distortionStrength = (sin(z) + 1.0) * (1.0 + 0.3 * sin(time * 0.3));
        float wavePattern = abs(sin(l * (9.0 + sin(time * 0.2) * 3.0) - z * 2.0));

        // Add mouse influence to distortion
        float mouseDist = length(p - (mouse - 0.5) * vec2(resolution.x / resolution.y, 1.0));
        float mouseInfluence = 0.0;
        if (mouseDist < 0.5)
        {
            mouseInfluence = (0.5 - mouseDist) * 2.0 * sin(time * 2.0);
        }

        // Apply enhanced distortion
        uv += p / max(0.001, l) * (distortionStrength + mouseInfluence) * wavePattern;

        // Add rotational flow
        float angle = atan(p.y, p.x);
        uv += 0.01 * vec2(cos(angle + time), sin(angle + time)) * sin(time + l * 5.0);

        // Calculate lightular-like pattern with enhanced effects
        vec2 lightCoord = mod(uv, 1.0) - 0.5;

        // Create multiple lightular layers
        float lightPattern = 0.01 / length(lightCoord);

        // Add fine details to lights
        lightPattern += 0.005 / length(mod(uv * 2.0, 1.0) - 0.5) * (0.5 + 0.5 * sin(l * 20.0 + time));

        // Store color with channel-specific effects
        if (i == 0)
        {
            // Red channel - warmer pattern
            c[i] = lightPattern * (0.9 + 0.3 * sin(time + l * 3.0));
        }
        else if (i == 1)
        {
            // Green channel - phase shifted
            c[i] = lightPattern * (0.8 + 0.3 * sin(time * 1.1 + l * 3.5));
        }
        else
        {
            // Blue channel - different phase
            c[i] = lightPattern * (0.7 + 0.4 * sin(time * 0.9 + l * 4.0));
        }
    }

    // Enhance color blending
    float intensity = max(l, 0.001) * (1.0 + 0.2 * sin(time));
    vec3 rawColor = c / intensity;

    // Color grading - add artistic color palette
    vec3 finalColor;

    // Apply color temperature shift based on position
    float temp = sin(fragCoord.x / resolution.x * 3.14159 + time * 0.5) * 0.5 + 0.5;

    // Create a vibrant color palette
    finalColor.r = rawColor.r * (1.0 + 0.3 * temp);
    finalColor.g = rawColor.g * (1.0 + 0.2 * (1.0 - temp));
    finalColor.b = rawColor.b * (1.0 + 0.3 * sin(temp * 3.14159));
    // Add subtle vignette effect
    vec2 vignettePos = fragCoord / resolution - 0.5;
    float vignette = 1.0 - dot(vignettePos, vignettePos) * 0.8;
    finalColor *= vignette;

    // Add subtle pulsing
    finalColor *= 0.8 + 0.2 * sin(time * 0.5);

    // Enhance contrast
    finalColor = pow(finalColor, vec3(0.9, 0.9, 0.9));

    // Convert to 8-bit color
    int r = int(clamp(finalColor.r * 255.0, 0.0, 255.0));
    int g = int(clamp(finalColor.g * 255.0, 0.0, 255.0));
    int b = int(clamp(finalColor.b * 255.0, 0.0, 255.0));
    // Store the final color
    data_0[idx] = 0xFF000000 | (r << 16) | (g << 8) | b;
}