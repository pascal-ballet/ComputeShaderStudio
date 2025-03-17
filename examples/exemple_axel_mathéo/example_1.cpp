float rand(uint x, uint y) {
    return fract(sin(float(x) * 12.9898 + float(y) * 78.233) * 43758.5453);
}

float rand2(float n) {
    return fract(sin(n) * 43758.5453123);
}

int rgb_to_int(int r, int g, int b) {
    return 0xFF000000 | (r << 16) | (g << 8) | b;
}

int rgb_to_int_alpha(int r, int g, int b, int a) {
    return (a << 24) | (r << 16) | (g << 8) | b;
}

int lerp_color(int c1, int r1, int g1, int b1, int r2, int g2, int b2, float t) {
    int r = int(float(r1) + t * float(r2 - r1));
    int g = int(float(g1) + t * float(g2 - g1));
    int b = int(float(b1) + t * float(b2 - b1));
    return rgb_to_int(r, g, b);
}

float water_height(float x, float time) {
    float wave1 = sin(x * 0.02 + time * 0.5) * 10.0;
    float wave2 = sin(x * 0.05 + time * 0.3) * 5.0;
    float wave3 = sin(x * 0.1 + time * 0.7) * 3.0;
    return wave1 + wave2 + wave3;
}

bool is_water(uint x, uint y, uint time, int screen_height, bool is_inside_circle) {
    if (is_inside_circle) {
        return false;
    }
    
    int water_start_y = int(screen_height * 0.7);
    if (int(y) < water_start_y) {
        return false;
    }
    float base_height = float(water_start_y);
    float wave_height = water_height(float(x), float(time) * 0.05);
    float surface_height = base_height + wave_height;
    return float(y) >= surface_height;
}

bool is_inside_circle(uint x, uint y, float cx, float cy, float radius) {
    float dx = float(x) - cx;
    float dy = float(y) - cy;
    float dist_sq = dx * dx + dy * dy;
    return dist_sq <= radius * radius;
}

int water_color(uint x, uint y, uint time, int screen_height, float circle_dist) {
    int base_r = 200;
    int base_g = 150;
    int base_b = 255;
    
    int water_start_y = int(screen_height * 0.7);
    float depth = float(y - water_start_y) / float(screen_height - water_start_y);
    depth = min(1.0, max(0.0, depth));
    
    base_b = int(float(base_b) * (1.0 - depth * 0.5));
    base_g = int(float(base_g) * (1.0 - depth * 0.7));
    
    float wave_height = water_height(float(x), float(time) * 0.05);
    float surface_height = float(water_start_y) + wave_height;
    float surface_dist = float(y) - surface_height;
    
    if (surface_dist >= -2.0 && surface_dist <= 2.0) {
        float wave_slope = water_height(float(x) + 1.0, float(time) * 0.05) - 
                           water_height(float(x) - 1.0, float(time) * 0.05);
        float foam_intensity = abs(wave_slope) * 0.2;
        
        base_r = int(mix(float(base_r), 255.0, foam_intensity));
        base_g = int(mix(float(base_g), 255.0, foam_intensity));
        base_b = int(mix(float(base_b), 255.0, foam_intensity));
    }
    
    if (circle_dist > 0.0) {
        float wave_effect = sin(float(time) * 0.2 + circle_dist * 10.0) * 0.5 + 0.5;
        float collision_foam = 0.7 + wave_effect * 0.3;
        
        collision_foam *= circle_dist;
        
        base_r = int(mix(float(base_r), 255.0, collision_foam));
        base_g = int(mix(float(base_g), 255.0, collision_foam));
        base_b = int(mix(float(base_b), 255.0, collision_foam));
    }
    
    float shimmer = sin(float(x) * 0.1 + float(y) * 0.1 + float(time) * 0.2) * 0.1 + 0.1;
    base_r = min(255, base_r + int(shimmer * 20.0));
    base_g = min(255, base_g + int(shimmer * 30.0));
    base_b = min(255, base_b + int(shimmer * 40.0));
    
    return rgb_to_int(base_r, base_g, base_b);
}

bool is_star(uint x, uint y, float density) {
    float r = rand(x, y);
    return r < density;
}

bool is_shooting_star(uint x, uint y, uint time, int num_stars) {
    for (int i = 0; i < num_stars; i++) {
        float seed = float(i) * 123.456 + float(i % 7) * 789.123;
        float time_offset = float(i) * 10.0;
        float offset_x = rand2(seed) * float(WSX);
        float offset_y = rand2(seed + 100.0) * float(WSY) * 0.5;
        float speed = 1.0 + rand2(seed + 200.0) * 2.0;
        float angle = 3.14159 * 0.25 + rand2(seed + 300.0) * 0.1 - 0.05;
        float cycle_time = float((time + uint(time_offset)) % 500);
        float current_x = offset_x + cos(angle) * speed * cycle_time;
        float current_y = offset_y + sin(angle) * speed * cycle_time;
        float tail_length = 5.0 + rand2(seed + 400.0) * 8.0;
        for (float t = 0.0; t < 1.0; t += 0.1) {
            float tail_x = current_x - cos(angle) * tail_length * t;
            float tail_y = current_y - sin(angle) * tail_length * t;
            float dist_sq = float((int(x) - int(tail_x)) * (int(x) - int(tail_x)) + 
                                 (int(y) - int(tail_y)) * (int(y) - int(tail_y)));
            if (dist_sq < (1.0 - t) * 2.0) {
                return true;
            }
        }
    }
    return false;
}

void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    int WHITE = 0xFFFFFFFF;
    int BLACK = 0xFF000000;
    int VIOLET = rgb_to_int(148, 0, 211);
    int BLUE = rgb_to_int(0, 0, 255);
    int ORANGE = rgb_to_int(255, 165, 0);
    int YELLOW = rgb_to_int(255, 255, 0);
    
    data_0[p] = BLACK;
    
    if (is_star(x, y, 0.001)) {
        float brightness = 0.5 + rand(x, y + 1000) * 0.5;
        int star_color = rgb_to_int(
            int(220.0 * brightness), 
            int(220.0 * brightness), 
            int(255.0 * brightness)
        );
        data_0[p] = star_color;
    }
    
    int cx = int(WSX) / 2;
    int cy = int(WSY) / 2;
    
    int dx = int(x) - cx;
    int dy = int(y) - cy;
    int distance_squared = dx * dx + dy * dy;
    float distance = sqrt(float(distance_squared));
    
    float angle = atan(float(dy), float(dx));
    if (angle < 0.0) angle += 3.14159 * 2.0;
    
    float angle_normalized = angle / (3.14159 * 2.0);
    
    int num_circles = 10;
    int max_radius = 200;
    int min_radius = 40;
    int thickness = 2;
    int frames_per_circle = 30;
    
    float radius_step = float(max_radius - min_radius) / float(num_circles - 1);
    
    int total_animation_frames = frames_per_circle * num_circles;
    int total_cycle_frames = total_animation_frames * 2;
    
    int cycle_step = step % total_cycle_frames;
    bool is_disappearing = cycle_step >= total_animation_frames;
    
    int current_step;
    if (is_disappearing) {
        current_step = total_animation_frames * 2 - cycle_step - 1;
    } else {
        current_step = cycle_step;
    }
    
    current_step = min(current_step, total_animation_frames - 1);
    
    int current_circle = current_step / frames_per_circle;
    int circle_progress = current_step % frames_per_circle;
    
    float completion_values[20];
    float start_angles[20];
    
    int safe_num_circles = min(num_circles, 20);
    
    for (int i = 0; i < safe_num_circles; i++) {
        completion_values[i] = 0.80 + rand(i, i) * (0.95 - 0.80);
        
        if (i == 0) {
            start_angles[i] = 0.0;
        } 
        else {
            if (i % 2 == (i-1) % 2) {
                start_angles[i] = completion_values[i-1];
            } 
            else {
                start_angles[i] = 1.0 - completion_values[i-1];
            }
        }
    }
    
    bool circle_drawn = false;
    float circle_distance = 0.0; 
    bool is_inside_any_circle = false;
    
    float circle_centers_x[20];
    float circle_centers_y[20];
    float circle_radii[20];
    int num_active_circles = 0;
    
    for (int i = 0; i <= current_circle && i < safe_num_circles; i++) {
        float current_radius = float(max_radius) - float(i) * radius_step;
        
        circle_centers_x[num_active_circles] = float(cx);
        circle_centers_y[num_active_circles] = float(cy);
        circle_radii[num_active_circles] = current_radius;
        num_active_circles++;
        
        if (is_inside_circle(x, y, float(cx), float(cy), current_radius + thickness)) {
            is_inside_any_circle = true;
        }
    }
    
    for (int i = 0; i < current_circle && i < safe_num_circles - 1; i++) {
        float current_radius = float(max_radius) - float(i) * radius_step;
        float next_radius = float(max_radius) - float(i+1) * radius_step;
        
        float end_angle, next_start_angle;
        
        if (i % 2 == 0) {
            end_angle = completion_values[i] * 2.0 * 3.14159;
        } else {
            end_angle = (1.0 - completion_values[i]) * 2.0 * 3.14159;
        }
        
        if ((i+1) % 2 == 0) {
            next_start_angle = start_angles[i+1] * 2.0 * 3.14159;
        } else {
            next_start_angle = (1.0 - start_angles[i+1]) * 2.0 * 3.14159;
        }
        
        float end_x = float(cx) + cos(end_angle) * current_radius;
        float end_y = float(cy) + sin(end_angle) * current_radius;
        
        float start_x = float(cx) + cos(next_start_angle) * next_radius;
        float start_y = float(cy) + sin(next_start_angle) * next_radius;
        
        circle_centers_x[num_active_circles] = end_x;
        circle_centers_y[num_active_circles] = end_y;
        circle_radii[num_active_circles] = float(thickness);
        num_active_circles++;
        
        circle_centers_x[num_active_circles] = start_x;
        circle_centers_y[num_active_circles] = start_y;
        circle_radii[num_active_circles] = float(thickness);
        num_active_circles++;
        
        float px = float(x);
        float py = float(y);
        
        float line_dx = start_x - end_x;
        float line_dy = start_y - end_y;
        float line_length_sq = line_dx * line_dx + line_dy * line_dy;
        
        float t = max(0.0, min(1.0, ((px - end_x) * line_dx + (py - end_y) * line_dy) / line_length_sq));
        
        float closest_x = end_x + t * line_dx;
        float closest_y = end_y + t * line_dy;
        
        float dist_sq = (px - closest_x) * (px - closest_x) + (py - closest_y) * (py - closest_y);
        
        if (dist_sq <= float(thickness * thickness)) {
            is_inside_any_circle = true;
        }
    }
    
    for (int i = 0; i <= current_circle && i < safe_num_circles; i++) {
        float current_radius = float(max_radius) - float(i) * radius_step;
        
        float inner_radius_sq = (current_radius - thickness) * (current_radius - thickness);
        float outer_radius_sq = (current_radius + thickness) * (current_radius + thickness);
        
        if (distance_squared >= inner_radius_sq && distance_squared <= outer_radius_sq) {
            float max_angle = completion_values[i];
            float start_angle = start_angles[i];
            
            if (i == current_circle) {
                float progress = float(circle_progress) / float(frames_per_circle);
                max_angle = start_angle + progress * (max_angle - start_angle);
            }
            
            float adjusted_angle;
            if (i % 2 == 0) {
                adjusted_angle = angle_normalized;
            } else {
                adjusted_angle = 1.0 - angle_normalized;
            }
            
            if (adjusted_angle >= start_angle && adjusted_angle <= max_angle) {
                float circle_position = float(i) / float(safe_num_circles - 1);
                
                int color;
                if (circle_position < 0.33) {
                    float t = circle_position / 0.33;
                    color = lerp_color(0, 148, 0, 211, 0, 0, 255, t);
                } else if (circle_position < 0.66) {
                    float t = (circle_position - 0.33) / 0.33;
                    color = lerp_color(0, 0, 0, 255, 255, 165, 0, t);
                } else {
                    float t = (circle_position - 0.66) / 0.34;
                    color = lerp_color(0, 255, 165, 0, 255, 255, 0, t);
                }
                
                data_0[p] = color;
                circle_drawn = true;
            }
        }
    }
    
    for (int i = 0; i < current_circle && i < safe_num_circles - 1; i++) {
        float current_radius = float(max_radius) - float(i) * radius_step;
        float next_radius = float(max_radius) - float(i+1) * radius_step;
        
        float end_angle, next_start_angle;
        
        if (i % 2 == 0) {
            end_angle = completion_values[i] * 2.0 * 3.14159;
        } else {
            end_angle = (1.0 - completion_values[i]) * 2.0 * 3.14159;
        }
        
        if ((i+1) % 2 == 0) {
            next_start_angle = start_angles[i+1] * 2.0 * 3.14159;
        } else {
            next_start_angle = (1.0 - start_angles[i+1]) * 2.0 * 3.14159;
        }
        
        float end_x = float(cx) + cos(end_angle) * current_radius;
        float end_y = float(cy) + sin(end_angle) * current_radius;
        
        float start_x = float(cx) + cos(next_start_angle) * next_radius;
        float start_y = float(cy) + sin(next_start_angle) * next_radius;
        
        float px = float(x);
        float py = float(y);
        
        float line_dx = start_x - end_x;
        float line_dy = start_y - end_y;
        float line_length_sq = line_dx * line_dx + line_dy * line_dy;
        
        float t = max(0.0, min(1.0, ((px - end_x) * line_dx + (py - end_y) * line_dy) / line_length_sq));
        
        float closest_x = end_x + t * line_dx;
        float closest_y = end_y + t * line_dy;
        
        float dist_sq = (px - closest_x) * (px - closest_x) + (py - closest_y) * (py - closest_y);
        
        if (dist_sq <= float(thickness * thickness)) {
            float circle_position = float(i) / float(safe_num_circles - 1);
            
            int color;
            if (circle_position < 0.33) {
                float t = circle_position / 0.33;
                color = lerp_color(0, 148, 0, 211, 0, 0, 255, t);
            } else if (circle_position < 0.66) {
                float t = (circle_position - 0.33) / 0.33;
                color = lerp_color(0, 0, 0, 255, 255, 165, 0, t);
            } else {
                float t = (circle_position - 0.66) / 0.34;
                color = lerp_color(0, 255, 165, 0, 255, 255, 0, t);
            }
            
            data_0[p] = color;
            circle_drawn = true;
        }
    }
    
    if (is_water(x, y, step, int(WSY), is_inside_any_circle)) {
        int water_col = water_color(x, y, step, int(WSY), circle_distance);
        
        if (circle_drawn) {
            int r = (water_col >> 16) & 0xFF;
            int g = (water_col >> 8) & 0xFF;
            int b = water_col & 0xFF;
            
            r = min(255, r + 150);
            g = min(255, g + 150);
            b = min(255, b + 150);
            
            water_col = rgb_to_int(r, g, b);
        }
        
        data_0[p] = water_col;
    }
    
    if (!circle_drawn && !is_water(x, y, step, int(WSY), is_inside_any_circle) && is_shooting_star(x, y, step, 50)) {
        data_0[p] = WHITE;
    }
}