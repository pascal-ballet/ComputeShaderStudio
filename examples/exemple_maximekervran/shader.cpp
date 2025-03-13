#define  CERCLE 0xFFFFFFFF
#define  FOND 0x00000000
#define RAYON 100.0
#define PI 3.141592


// Uniforme qui sera passé depuis le script GDScript
layout(set = 0, binding = 0) uniform Params {
    bool mouse_clicked;
    vec2 mouse_position;
};


void cercle(uint cy, uint cx){
    uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    float distance = sqrt(float((x - cx) * (x - cx) + (y - cy) * (y - cy)));
    
    if (distance <= RAYON) {
        data_0[p] = CERCLE; // Pixel blanc
    } else {
        data_0[p] = FOND;   // Pixel noir (fond)
    }
}

void main() {
    uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    data_0[p] = FOND;

    if (mouse_clicked) {
        cercle(uint(mouse_position.y), uint(mouse_position.x));
    }
}


/*void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;
    int opacity = 0xFF000000 ;
    int blue = 0x550000;
    int green = 0x3300;
    int red = int(y*2) ;
    data_0[p] = opacity+blue+green+red;
}*/


/*void button()
 var btn : int = 1;
 input.param.append(Input.get_mouse_button_mask())//renvoie 1 si bouton gauche cliqué
if(Input.is_mouse_button_pssed(Mouse_button_left))*/