#define CERCLE 0xFFFFFFFF
#define FOND 0x00000000
#define RAYON 50.0
#define PI 3.141592

void cercle(uint cy, uint cx) {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    float distance = sqrt(float((x - cx) * (x - cx) + (y - cy) * (y - cy)));
    
    if (distance <= RAYON) {
        data_0[p] = CERCLE; // White pixel
    } else {
        data_0[p] = FOND;   // Black pixel (background)
    }
}

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    // Clear screen
    data_0[p] = FOND;

    // Ball position
    uint ball_x = 250;
    
    // Calculate y position with bounce effect
    float time = float(step);
    float gravity = 0.5;
    float topHeight = 50.0;
    float bottomHeight = float(WSY - uint(RAYON));
    
    // Simple bounce calculation
    float bounce_period = 90.0; // Time for one complete bounce cycle
    float bounce_decay = 0.85;   // How much energy is lost per bounce
    
    // Calculate based on sine wave with decreasing amplitude
    float cycles = time / bounce_period;
    float amplitude = (bottomHeight - topHeight) * pow(bounce_decay, floor(cycles));
    float phase = (cycles - floor(cycles)) * 2.0 * PI;
    
    // Position calculation: start at bottom, bounce up with decreasing height
    //float y_pos = maxHeight - amplitude * (0.5 + 0.5 * sin(phase - PI/2.0));
    
    // Position calculation: start at top, fall down to bottom, then bounce
    float y_pos;
// For the first cycle, use a simple gravity equation to make it fall naturally
    if (cycles < 1.0) {
        // Simple gravity equation: y = y0 + v0*t + 0.5*g*t²
        float normalized_time = cycles; // Between 0 and 1
        y_pos = topHeight + (bottomHeight - topHeight) * normalized_time * normalized_time;
        
        // Ensure it doesn't go past the bottom
        if (y_pos > bottomHeight) {
            y_pos = bottomHeight;
        }
    } else {
        // After first impact, use the sine wave for bouncing
        y_pos = bottomHeight - amplitude * (0.5 + 0.5 * sin(phase - PI/2.0));
    }

    // Draw the ball
    cercle(uint(y_pos), ball_x);
}