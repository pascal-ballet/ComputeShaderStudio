// Format RGBA
#define RED 0xFF0000FF        // Red
#define GREEN 0xFF00FF00      // Green 
#define BLUE 0xFFFF0000       // Blue
#define YELLOW 0xFFFFFF00     // Yellow
#define BLACK 0xFF000000      // Black
#define WHITE 0xFFFFFFFF      // White

// Alternative ARGB format (uncomment if needed)
//#define RED 0xFFFF0000      // Red
//#define GREEN 0xFF00FF00    // Green
//#define BLUE 0xFF0000FF     // Blue
//#define YELLOW 0xFFFFFF00   // Yellow
//#define BLACK 0xFF000000    // Black
//#define WHITE 0xFFFFFFFF    // White

#define PLAYER_SIZE 5         // Player radius
#define SPAWN_RATE 50         // Base enemy spawn rate
#define ENEMY_SPEED 0.8       // Base movement speed
#define DIFFICULTY_INC 100    // Frames per difficulty increase

// Score display config
#define SCORE_Y 10
#define DIGIT_WIDTH 8 
#define DIGIT_HEIGHT 12
#define DIGIT_SPACING 10

int hash(int x, int y, int s) {
    int a = x * 0x6A09E667;
    int b = y * 0xBB67AE85;
    int c = s * 0x3C6EF372;
    int h = a + b + c;
    h = (h ^ (h >> 16)) * 0x85EBCA6B;
    h = (h ^ (h >> 13)) * 0xC2B2AE35;
    h = h ^ (h >> 16);
    return h;
}

bool random_event(int x, int y, int step, float probability) {
    int random_value = hash(x, y, step);
    float normalized = float(random_value) / 2147483647.0;
    return (normalized > 0.0 ? normalized : -normalized) < probability;
}

void drawHLine(int startX, int y, int length, int color) {
    for(int i = 0; i < length; i++) {
        int pos = (startX + i) + y * int(WSX);
        if(startX + i >= 0 && startX + i < int(WSX) && y >= 0 && y < int(WSY)) {
            data_0[pos] = color;
        }
    }
}

void drawVLine(int x, int startY, int length, int color) {
    for(int i = 0; i < length; i++) {
        int pos = x + (startY + i) * int(WSX);
        if(x >= 0 && x < int(WSX) && startY + i >= 0 && startY + i < int(WSY)) {
            data_0[pos] = color;
        }
    }
}

void drawNumber(int num, int x, int y, int color) {
    if (num == 0) {
        drawHLine(x, y, 5, color);
        drawHLine(x, y+6, 5, color);
        drawVLine(x, y, 7, color);
        drawVLine(x+4, y, 7, color);
        return;
    }
    
    int digits[10];
    int digit_count = 0;
    int temp = num;
    
    while (temp > 0) {
        digits[digit_count] = temp % 10;
        temp /= 10;
        digit_count++;
    }
    
    for (int i = 0; i < digit_count; i++) {
        int digit = digits[digit_count - i - 1];
        int dx = x + i * 8;
        
        if (digit == 0) {
            drawHLine(dx, y, 5, color);
            drawHLine(dx, y+6, 5, color);
            drawVLine(dx, y, 7, color);
            drawVLine(dx+4, y, 7, color);
        }
        else if (digit == 1) {
            drawVLine(dx+2, y, 7, color);
        }
        else if (digit == 2) {
            drawHLine(dx, y, 5, color);
            drawHLine(dx, y+3, 5, color);
            drawHLine(dx, y+6, 5, color);
            drawVLine(dx+4, y, 4, color);
            drawVLine(dx, y+3, 4, color);
        }
        else if (digit == 3) {
            drawHLine(dx, y, 5, color);
            drawHLine(dx, y+3, 5, color);
            drawHLine(dx, y+6, 5, color);
            drawVLine(dx+4, y, 7, color);
        }
        else if (digit == 4) {
            drawVLine(dx, y, 4, color);
            drawVLine(dx+4, y, 7, color);
            drawHLine(dx, y+3, 5, color);
        }
        else if (digit == 5) {
            drawHLine(dx, y, 5, color);
            drawHLine(dx, y+3, 5, color);
            drawHLine(dx, y+6, 5, color);
            drawVLine(dx, y, 4, color);
            drawVLine(dx+4, y+3, 4, color);
        }
        else if (digit == 6) {
            drawHLine(dx, y, 5, color);
            drawHLine(dx, y+3, 5, color);
            drawHLine(dx, y+6, 5, color);
            drawVLine(dx, y, 7, color);
            drawVLine(dx+4, y+3, 4, color);
        }
        else if (digit == 7) {
            drawHLine(dx, y, 5, color);
            drawVLine(dx+4, y, 7, color);
        }
        else if (digit == 8) {
            drawHLine(dx, y, 5, color);
            drawHLine(dx, y+3, 5, color);
            drawHLine(dx, y+6, 5, color);
            drawVLine(dx, y, 7, color);
            drawVLine(dx+4, y, 7, color);
        }
        else if (digit == 9) {
            drawHLine(dx, y, 5, color);
            drawHLine(dx, y+3, 5, color);
            drawHLine(dx, y+6, 5, color);
            drawVLine(dx, y, 4, color);
            drawVLine(dx+4, y, 7, color);
        }
    }
}

void main() {
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    int p = x + y * int(WSX);
    
    // Simplified color test
    if (step < 5) {
        // Simple color grid test
        if (x < int(WSX)/2) {
            if (y < int(WSY)/2) {
                data_0[p] = GREEN;  // Top-left: green
            } else {
                data_0[p] = YELLOW; // Bottom-left: yellow
            }
        } else {
            if (y < int(WSY)/2) {
                data_0[p] = BLUE;   // Top-right: blue
            } else {
                data_0[p] = WHITE;  // Bottom-right: white
            }
        }
        
        // Black grid lines
        if (x == int(WSX)/2 || y == int(WSY)/2) {
            data_0[p] = BLACK;
        }
        
        // Player (red circle)
        if (mousex >= 0 && mousey >= 0) {
            float dist = sqrt(float((x - mousex) * (x - mousex) + (y - mousey) * (y - mousey)));
            if (dist < 20) {
                data_0[p] = RED;
            }
        }
        
        return;
    }
    
    // Game initialization
    if (step == 5) {
        // Reserve storage area (first line)
        if (y == 0 && x < 10) {
            if (x == 0) data_0[p] = 1;      // Game active flag (1=active, 0=over)
            else if (x == 1) data_0[p] = 0;  // Score (survival time)
            else if (x == 2) data_0[p] = 0;  // Difficulty
            else data_0[p] = 0;
        } else {
            data_0[p] = BLACK;
        }
    } else {
        // Check game state
        bool game_active = (data_0[0] == 1);
        int score = data_0[1];
        int difficulty = data_0[2];
        
        // Update game state (only at 0,0)
        if (x == 0 && y == 0) {
            // Increase score if game is active
            if (game_active) {
                data_0[1]++; // Increment score
                
                // Increase difficulty periodically
                if (step % DIFFICULTY_INC == 0) {
                    data_0[2]++;
                }
            }
        }
        
        // Clear screen (except storage area)
        if (!(y == 0 && x < 10)) {
            data_0[p] = BLACK;
        }
        
        // Active game logic
        if (game_active) {
            // Check if player still exists
            if (x == 0 && y == 1) {
                bool player_alive = false;
                for (int j = 0; j < int(WSY); j++) {
                    for (int i = 0; i < int(WSX); i++) {
                        int idx = i + j * int(WSX);
                        if (data_0[idx] == RED) {
                            player_alive = true;
                            break;
                        }
                    }
                    if (player_alive) break;
                }
                
                // If player is gone, game over
                if (!player_alive && score > 10) { 
                    data_0[0] = 0;
                }
            }
            
            // Draw player around mouse
            if (mousex >= 0 && mousey >= 0) {
                float dist = sqrt(float((x - mousex) * (x - mousex) + (y - mousey) * (y - mousey)));
                if (dist < PLAYER_SIZE) {
                    data_0[p] = RED;
                }
            }
            
            // Spawn enemies from edges
            if ((step + x + y) % max(5, SPAWN_RATE - difficulty) == 0) {
                // Choose random edge
                int border = hash(x, y, step) % 4; // 0=top, 1=right, 2=bottom, 3=left
                
                // Edge positions
                if ((border == 0 && y == 0) ||
                    (border == 1 && x == int(WSX)-1) ||
                    (border == 2 && y == int(WSY)-1) ||
                    (border == 3 && x == 0)) {
                    
                    // Spawn probability inversely proportional to edge length
                    int border_length = (border % 2 == 0) ? int(WSX) : int(WSY);
                    
                    if (random_event(x, y, step, 2.0 / float(border_length))) {
                        // Enemy type based on difficulty
                        int enemy_type = hash(x, y, step) % 3;
                        if (difficulty < 10) enemy_type = 0;
                        else if (difficulty < 20) enemy_type = hash(x, y, step) % 2;
                        
                        switch(enemy_type) {
                            case 0: data_0[p] = GREEN; break;
                            case 1: data_0[p] = BLUE; break;
                            case 2: data_0[p] = YELLOW; break;
                        }
                    }
                }
            }
            
            // Move enemies toward player
            if (data_0[p] == GREEN || data_0[p] == BLUE || data_0[p] == YELLOW) {
                // Calculate direction to player (or center if no mouse)
                int target_x = (mousex >= 0) ? mousex : int(WSX) / 2;
                int target_y = (mousey >= 0) ? mousey : int(WSY) / 2;
                
                float dx = float(target_x - x);
                float dy = float(target_y - y);
                float dist = sqrt(dx*dx + dy*dy);
                
                if (dist > 0) {
                    // Normalize
                    dx /= dist;
                    dy /= dist;
                    
                    // Speed based on type and difficulty
                    float speed = ENEMY_SPEED;
                    if (data_0[p] == BLUE) speed *= 1.2;
                    if (data_0[p] == YELLOW) speed *= 1.5;
                    
                    // Add speed bonus based on difficulty
                    speed *= (1.0 + float(difficulty) / 50.0);
                    
                    // New position
                    int new_x = int(float(x) + dx * speed);
                    int new_y = int(float(y) + dy * speed);
                    
                    // Check boundaries
                    if (new_x >= 0 && new_x < int(WSX) && new_y >= 0 && new_y < int(WSY)) {
                        int new_p = new_x + new_y * int(WSX);
                        
                        // Collision with player = game over
                        if (data_0[new_p] == RED) {
                            data_0[new_p] = GREEN; // Replace player
                        } 
                        // Otherwise move enemy
                        else if (data_0[new_p] == BLACK) {
                            data_0[new_p] = data_0[p];
                            data_0[p] = BLACK;
                        }
                    }
                }
            }
            
            // Display score in top-right
            if (x == int(WSX) - 50 && y == 10) {
                drawNumber(score, x, y, WHITE);
            }
        } 
        // Game over screen
        else {
            // Draw GAME OVER message
            if (y == int(WSY)/2 - 10 && x == int(WSX)/2 - 50) {
                // Simple line message
                drawHLine(x, y, 100, WHITE);
                drawHLine(x, y+7, 100, WHITE);
                drawHLine(x, y+15, 100, WHITE);
            }
            
            // Show final score
            if (y == int(WSY)/2 + 10 && x == int(WSX)/2 - 20) {
                drawNumber(score, x, y, WHITE);
            }
        }
    }
}