void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y*WSX;
    
    // Normalize coordinates
    vec2 fragCoord = vec2(float(x), float(y));
    vec2 r = vec2(float(WSX), float(WSY));
    vec2 uv = fragCoord/r;
    vec2 centered = uv - 0.5;
    centered.x *= r.x/r.y;
    
    // Time control
    float t = float(step) * 0.03;
    
    // Generate random neuron centers
    #define NEURON_COUNT 12
    vec2 neuronCenters[NEURON_COUNT];
    float neuronPhases[NEURON_COUNT];
    float neuronStrengths[NEURON_COUNT];
    
    // Initialize with pseudorandom positions using trig functions with different seeds
    for (int i = 0; i < NEURON_COUNT; i++) {
        // Use prime numbers and different bases to create "random" distribution
        float randX = sin(float(i) * 12.9898 + t * 0.05) * 0.5;
        float randY = cos(float(i) * 78.233 + t * 0.04) * 0.5;
        
        // Add some slow movement to neurons
        randX += sin(t * 0.07 + float(i) * 0.5) * 0.1;
        randY += cos(t * 0.05 + float(i) * 0.7) * 0.1;
        
        neuronCenters[i] = vec2(randX, randY);
        
        // Random phase and firing strength for each neuron
        neuronPhases[i] = t * (0.8 + sin(float(i) * 3.14) * 0.3) + float(i);
        neuronStrengths[i] = 0.5 + sin(float(i) * 1.618 + t * 0.2) * 0.5;
    }
    
    // ==== RANDOM CONNECTION DETERMINATION ====
    bool connections[NEURON_COUNT][NEURON_COUNT];
    for (int i = 0; i < NEURON_COUNT; i++) {
        for (int j = 0; j < NEURON_COUNT; j++) {
            // Use a deterministic but "random-looking" pattern for connections
            // About 30% of possible connections will be active
            float connectionSeed = sin(float(i) * 43.21 + float(j) * 12.345) * 0.5 + 0.5;
            connections[i][j] = (i != j) && (connectionSeed < 0.3);
        }
    }
    
    // ==== NEURAL PATHWAY GENERATION ====
    float neuralStructure = 0.0;
    float activationMap = 0.0;
    float synapseMap = 0.0;
    
    for (int i = 0; i < NEURON_COUNT; i++) {
        for (int j = 0; j < NEURON_COUNT; j++) {
            if (connections[i][j]) {
                // Create paths between connected neurons
                vec2 startPos = neuronCenters[i];
                vec2 endPos = neuronCenters[j];
                vec2 pathDir = normalize(endPos - startPos);
                
                // Generate path with random variation
                float pathLength = length(endPos - startPos);
                float pathDist = 0.0;
                
                // Connection strength varies with distance
                float connectionStrength = 1.0 - smoothstep(0.0, 0.7, pathLength);
                
                // Define path with random bezier curve
                float t_path = clamp(dot(centered - startPos, pathDir) / pathLength, 0.0, 1.0);
                
                // Calculate bezier control point (randomly offset perpendicular to path)
                float randOffset = sin(float(i) * 13.37 + float(j) * 42.42) * 0.15;
                vec2 perp = vec2(-pathDir.y, pathDir.x);
                vec2 controlPoint = mix(startPos, endPos, 0.5) + perp * randOffset;
                
                // Quadratic bezier formula
                vec2 pathPos = mix(
                    mix(startPos, controlPoint, t_path), 
                    mix(controlPoint, endPos, t_path), 
                    t_path
                );
                
                // Add smaller random variations along path
                float noiseScale = 10.0;
                float noiseStrength = 0.01;
                float pathNoise = sin(t_path * noiseScale + float(i+j)) * 
                                cos(t_path * noiseScale * 1.5 + float(i-j));
                pathPos += perp * pathNoise * noiseStrength;
                
                // Calculate distance to path
                pathDist = length(centered - pathPos);
                
                // Path thickness varies along length - thinner in middle
                float baseThickness = 0.003 * connectionStrength;
                float pathThickness = baseThickness * (1.0 + sin(t_path * 3.14) * 0.5);
                
                // Add some variation to thickness
                pathThickness *= (0.8 + sin(t_path * 20.0 + float(i+j)) * 0.2);

                // Add neural structure to the map
                float pathContribution = smoothstep(pathThickness, 0.0, pathDist);
                neuralStructure = max(neuralStructure, pathContribution);
                
                // Define synapses near connection points
                float synapseDist = length(centered - endPos);
                float synapseSize = 0.01 + 0.005 * sin(float(j) * 7.77);
                synapseMap = max(synapseMap, smoothstep(synapseSize, 0.0, synapseDist));
                
                // ==== PULSE PROPAGATION ====
                float pulsePhase = neuronPhases[i];
                
                // Pulses emit when source neuron fires (based on sin wave)
                float firingThreshold = 0.7;
                float neuronFiring = (sin(pulsePhase) * 0.5 + 0.5) > firingThreshold ? 1.0 : 0.0;
                
                if (neuronFiring > 0.0) {
                    // Create a pulse that travels from soma to terminal
                    float pulseSpeed = 1.0 + sin(float(i) * 0.7) * 0.2; // Slightly different speed per neuron
                    float pulseTiming = mod(t * pulseSpeed - t_path + 2.0, 1.0);
                    
                    // Only show pulse when it's on this segment
                    if (pulseTiming < 0.3) {
                        // Pulse shape (compact wave)
                        float pulseShape = smoothstep(0.0, 0.05, pulseTiming) * smoothstep(0.3, 0.1, pulseTiming);
                        
                        // Intensity varies with neuron strength and fades with distance
                        float pulseIntensity = pulseShape * neuronStrengths[i] * connectionStrength;
                        
                        // Only show pulse on the path
                        float activationContribution = pulseIntensity * smoothstep(pathThickness * 2.0, 0.0, pathDist);
                        activationMap = max(activationMap, activationContribution);
                    }
                }
            }
        }
    }
    
    // ==== CELL BODIES (SOMAS) ====
    float somaBrightness = 0.0;
    for (int i = 0; i < NEURON_COUNT; i++) {
        float somaDist = length(centered - neuronCenters[i]);
        
        // Random variable cell body sizes
        float somaSize = 0.01 + 0.008 * sin(float(i) * 9.23);
        float somaShape = smoothstep(somaSize, 0.0, somaDist);
        
        // Firing effect - each neuron pulses with its own rhythm
        float firingEffect = (sin(neuronPhases[i]) * 0.5 + 0.5) * neuronStrengths[i];
        somaBrightness = max(somaBrightness, somaShape * (0.4 + firingEffect));
    }
    
    // ==== ORGANIC BACKGROUND TEXTURE ====
    float bgNoise = 0.0;
    for (int f = 1; f <= 4; f++) {
        float scale = float(f) * 8.0;
        // Use primes to get more chaotic patterns
        bgNoise += (sin(centered.x * scale * 1.13 + t + sin(centered.y * scale * 0.97)) * 
                   sin(centered.y * scale * 1.29 + t * 0.77 + sin(centered.x * scale * 1.37))) 
                   * (0.4 / float(f));
    }
    bgNoise = bgNoise * 0.5 + 0.5; // Normalize to 0-1 range
    
    // Mouse interaction - create a stimulation point
    float mouseStimulation = 0.0;
    if (mousex > 0 && mousey > 0) {
        vec2 mousePos = vec2(float(mousex)/r.x, float(mousey)/r.y) - 0.5;
        mousePos.x *= r.x/r.y;
        float mouseDist = length(centered - mousePos);
        
        // Create spreading wave of activation
        float waveSpeed = 0.5;
        float waveRadius = mod(t * waveSpeed, 1.0) * 0.5;
        float waveWidth = 0.02;
        float activationRing = smoothstep(waveWidth, 0.0, abs(mouseDist - waveRadius));
        mouseStimulation = activationRing * exp(-waveRadius * 5.0);
        
        // Excite nearby neurons
        for (int i = 0; i < NEURON_COUNT; i++) {
            float neuronDist = length(mousePos - neuronCenters[i]);
            if (neuronDist < 0.2) {
                float influence = (0.2 - neuronDist) * 5.0;
                neuronPhases[i] += influence * 0.2; // Increase firing rate
                neuronStrengths[i] = min(1.0, neuronStrengths[i] + influence * 0.1); // Increase strength
            }
        }
    }
    
    // ==== VISUAL EFFECTS ====
    
    // Base neural structure color with subtle variation
    vec3 structureBase = vec3(0.02, 0.04, 0.12);
    vec3 structureHighlight = vec3(0.1, 0.2, 0.4);
    vec3 structureColor = mix(
        structureBase,
        structureHighlight,
        neuralStructure
    );
    
    // Add subtle color variation based on position
    structureColor += vec3(0.05, -0.02, 0.0) * sin(centered.x * 5.0 + t);
    structureColor += vec3(-0.02, 0.0, 0.05) * sin(centered.y * 4.0 + t * 0.7);
    
    // Pulse color - electric blue/cyan
    vec3 pulseBaseColor = vec3(0.0, 0.4, 0.8);
    vec3 pulseBrightColor = vec3(0.2, 0.9, 1.0);
    vec3 pulseColor = mix(
        pulseBaseColor,
        pulseBrightColor,
        activationMap
    );
    
    // Soma color with random variations
    vec3 somaBaseColor = vec3(0.3, 0.4, 0.6);
    vec3 somaBrightColor = vec3(0.7, 0.8, 1.0);
    vec3 somaColor = mix(
        somaBaseColor,
        somaBrightColor,
        somaBrightness
    );
    
    // Synapse flashing
    vec3 synapseColor = vec3(0.2, 0.6, 1.0);
    
    // Combine all elements
    vec3 finalColor = vec3(0.0);
    
    // Add subtle background neural texture
    finalColor += vec3(0.01, 0.02, 0.04) * bgNoise;
    
    // Add base neural structure
    finalColor += structureColor * (neuralStructure * 0.6 + 0.3);
    
    // Add pulse effect with bright glow
    finalColor += pulseColor * pow(activationMap * 1.5, 2.0) * 2.0;
    
    // Add soma glow
    finalColor += somaColor * somaBrightness * 1.5;
    
    // Add synapse flash
    finalColor += synapseColor * synapseMap * (sin(t * 3.0) * 0.5 + 0.7);
    
    // Add mouse stimulation with electric blue color
    finalColor += vec3(0.0, 0.6, 1.0) * mouseStimulation * 2.0;
    
    // Overall glow effect
    finalColor += structureColor * 0.1 * (sin(t * 0.5) * 0.3 + 0.7);
    
    // Enhance contrast and brightness
    finalColor = pow(finalColor, vec3(0.85));
    
    // Convert to RGBA color format for data_0
    int red = int(min(finalColor.r * 255.0, 255.0));
    int green = int(min(finalColor.g * 255.0, 255.0));
    int blue = int(min(finalColor.b * 255.0, 255.0));
    
    data_0[p] = int(0xFF000000 | (red << 16) | (green << 8) | blue);
}