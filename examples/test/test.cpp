#version 450

// Workgroup size
layout(local_size_x = 16, local_size_y = 16) in;

void main() 
{
    // Get the current pixel coordinates
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    
    // Input parameters (these are just placeholders)
    vec2 resolution = vec2(800.0, 600.0); // Example resolution
    float time = 1.0; // Example time (you can replace this with an actual uniform in your app)

    // Check if within bounds
    if(pixelCoords.x >= int(resolution.x) || pixelCoords.y >= int(resolution.y)) {
        return;
    }
    
    // Normalized pixel coordinates (from 0 to 1)
    vec2 fragCoord = vec2(pixelCoords);
    vec2 uv = (fragCoord * 2.0 - resolution.xy) / resolution.y;
    
    // Time loop
    float animTime = abs(8.0 - mod(time + 8.0, 16.0));
        
    // Side lengths for demo
    float edge_x = (0.6 + 0.4 * cos(time)) * smoothstep(1.0, 2.0, animTime);
    float edge_y = (0.6 + 0.4 * sin(time)) * smoothstep(3.0, 4.0, animTime);
    
    // Hypotenuse edge
    vec2 edge_h = vec2(edge_x, -edge_y) * smoothstep(5.0, 6.0, animTime);
    
    // Centered position coordinates
    vec2 p = uv + 0.5 * vec2(edge_x, edge_y);
    
    // Distance to x-edge
    float d = length(p - clamp(p, vec2(0.0), vec2(edge_x, 0)));
    
    // Distance to y-edge
    d = min(d, length(p - clamp(p, vec2(0.0), vec2(0, edge_y))));
    
    // Hypotenuse edge distance
    p -= vec2(0, edge_y);
    float h = clamp(dot(p, edge_h) / dot(edge_h, edge_h), 0.0, 1.0);
    d = min(d, length(p - edge_h * h));
    
    // Compute distance field sign
    float ex = -p.x;
    float ey = -edge_y - p.y;
    float eh = edge_y * p.x + edge_x * p.y;
    
    // Use the sign of the greatest edge
    float sig = sign(max(ex, max(ey, eh)));
    sig = mix(1.0, sig, smoothstep(6.0, 7.0, animTime));
    
    // Isolines
    vec3 col = 0.95 + 0.05 * cos(d * 1e2 + vec3(0, 1, 2));
    col /= 1.0 + 20.0 * d * exp(sig * vec3(3, 0, -1));
    
    // Output color (write to the image)
    imageStore(image2D, pixelCoords, vec4(col, 1.0));
}






























// #define RADIUS 10
// #define MAX_LENGTH 50


// void main()
// {
//     int x = int(gl_GlobalInvocationID.x);
//     int y = int(gl_GlobalInvocationID.y);
//     int p = x + y * int(WSX);
//     int positions[MAX_LENGTH][2]; // Tableau pour stocker les positions du serpent

//     for (int i = 0; i < MAX_LENGTH; ++i)
//     {
//         positions[i][0] = 0;
//         positions[i][1] = 0;
//     }

//     // Initial background is black & opaque
//     if (step == 0)
//     {
//         data_0[p] = 0xFF000000 + y;
//     }
//     else
//     {
//         // Déplacer le serpent en fonction de la souris
//         for (int i = MAX_LENGTH - 1; i > 0; --i)
//         {
//             positions[i][0] = positions[i - 1][0];
//             positions[i][1] = positions[i - 1][1];
//         }

//         // Stocke la nouvelle position de la souris
//         positions[0][0] = mousex;
//         positions[0][1] = mousey;

//         // Dessiner chaque segment du serpent
//         for (int i = 0; i < MAX_LENGTH; ++i)
//         {
//             float dx = float(positions[i][0] - x);
//             float dy = float(positions[i][1] - y);
//             float dist = sqrt(dx * dx + dy * dy);
            
//             // Dessiner un segment du serpent
//             if (dist < RADIUS)
//             {
//                 int col = 0xFF00FF00; // Vert pour le serpent
//                 data_0[p] = 0xFF000000 + col;
//             }
//         }
//     }

//     step++;
// }











// #define RADIUS 40

// void main()
// {
//     int x = int(gl_GlobalInvocationID.x);
//     int y = int(gl_GlobalInvocationID.y);
//     int p = x + y * int(WSX);

//     // Initial background is black & opaque
//     if (step == 0)
//     {
//         data_0[p] = 0xFF000000 + y;
//     }else{
//         // Draw a fading red circle
//         float dx = float(mousex - x);
//         float dy = float(mousey - y);
//         float dist = sqrt(dx * dx + dy * dy);

//         int pix = data_0[p];
//         int col = pix & 0x00FFFFFF;
//         float r = RADIUS + 10 * sin(step / 10.0f);
//         if (dist < r - 1)
//         {
//             col = col + 0x04;
//             data_0[p] = 0xFF000000 + col;
//         }

//     }
// }






























// void main() {
//     uint x = gl_GlobalInvocationID.x;
//     uint y = gl_GlobalInvocationID.y;
//     uint p = x + y * WSX; // Indexation en 1D d'une matrice 2D

//     int row = y % 2; // Alternance de lignes
//     int col = x % 2; // Alternance de colonnes

//     data_0[p] = (row == col) ? 0xFFFFFF00 : 0xFF0000FF; // Motif damier
//     data_1[p] =  0xFFFF0000 - int(x + y) * (step); 
// }



// void main() {
// 	uint x = gl_GlobalInvocationID.x;
// 	uint y = gl_GlobalInvocationID.y;
// 	uint p = x + y * WSX;
// 	data_0[p] = 0xFFFF0000 - int(x+y*1024)*(step);
// }

