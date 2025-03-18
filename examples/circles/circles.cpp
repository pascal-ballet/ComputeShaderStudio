shader_type canvas_item;

// Godot uniform variables
uniform vec2 resolution = vec2(1280.0, 720.0);
uniform float time_scale = 1.0;

// Helper function: Distance to a line segment
float lineSegmentDistance(vec2 p, vec2 a, vec2 b) {
    vec2 ap = p - a;
    vec2 ab = b - a;
    float t = clamp(dot(ap, ab) / dot(ab, ab), 0.0, 1.0);
    return length(ap - ab * t);
}

void fragment() {
    // Normalize screen coordinates
    vec2 uv = FRAGCOORD.xy / resolution.xy;
    uv -= 0.5;
    uv.x *= resolution.x / resolution.y; // Fix aspect ratio
    
    // Time control
    float t = TIME * time_scale;
    
    // Background color - adding a subtle gradient
    vec3 finalColor = vec3(0.03, 0.03, 0.05) + 0.02 * vec3(1.0-length(uv));
    
    // Define neuron positions (6 neurons in a hexagon pattern)
    vec2 neurons[6];
    neurons[0] = vec2(-0.4, -0.3);
    neurons[1] = vec2(0.0, -0.5);
    neurons[2] = vec2(0.4, -0.3);
    neurons[3] = vec2(-0.4, 0.3);
    neurons[4] = vec2(0.0, 0.5);
    neurons[5] = vec2(0.4, 0.3);
    
    // Draw neurons and connections
    for (int i = 0; i < 6; i++) {
        // Add some subtle movement to neurons
        vec2 neuronPos = neurons[i] + vec2(
            sin(t * 0.5 + float(i) * 0.5) * 0.02,
            cos(t * 0.4 + float(i) * 0.7) * 0.02
        );
        
        // Draw neurons (glowing red dots)
        float distToNeuron = length(uv - neuronPos);
        float neuronGlow = 0.03 / (distToNeuron + 0.01);
        float neuronPulse = 0.5 + 0.5 * sin(t * 0.7 + float(i) * 1.1);
        finalColor += vec3(1.0, 0.2, 0.2) * neuronGlow * neuronPulse;
        
        // Draw connections between neurons
        for (int j = i + 1; j < 6; j++) {
            vec2 neuronA = neuronPos;
            vec2 neuronB = neurons[j] + vec2(
                sin(t * 0.5 + float(j) * 0.5) * 0.02,
                cos(t * 0.4 + float(j) * 0.7) * 0.02
            );
            
            // Draw nerve fibers (blue lines)
            float distToFiber = lineSegmentDistance(uv, neuronA, neuronB);
            float fiberGlow = 0.002 / (distToFiber + 0.001);
            finalColor += vec3(0.2, 0.2, 0.8) * fiberGlow * 0.3;
            
            // Animate impulses (yellow dots) along the fibers
            float impulseSpeed = 0.8;
            float impulseTime = fract(t * impulseSpeed + float(i * 3 + j) * 0.1);
            vec2 impulsePos = mix(neuronA, neuronB, impulseTime);
            float distToImpulse = length(uv - impulsePos);
            float impulseGlow = 0.008 / (distToImpulse + 0.003);
            finalColor += vec3(1.0, 1.0, 0.0) * impulseGlow * 2.0;
            
            // Add a secondary impulse going the other way
            float impulseTime2 = fract(t * impulseSpeed * 0.7 + float(i * 5 + j * 2) * 0.15);
            vec2 impulsePos2 = mix(neuronB, neuronA, impulseTime2);
            float distToImpulse2 = length(uv - impulsePos2);
            float impulseGlow2 = 0.008 / (distToImpulse2 + 0.003);
            finalColor += vec3(0.0, 1.0, 0.8) * impulseGlow2 * 2.0;
        }
    }
    
    // Add some bloom effect
    finalColor = pow(finalColor, vec3(0.7));
    
    // Output to screen
    COLOR = vec4(finalColor, 1.0);
}