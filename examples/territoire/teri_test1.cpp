
#define ALIVE 0xFFFFFFFF
#define DEAD 0xFF0000FF

void main() {
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	uint p = x + y * WSX;
	if ( step == 0 ) { // initialisation ************************
		if ( current_pass == 0 ) {
			data_1[p] = DEAD ;
			if (data_0[p] < 0 || x==0 || y==0 || x==WSX-1 || y==WSY-1)
				data_0[p] = DEAD ;
			else
				data_0[p] = ALIVE ;
		}
	} else { // in process *********************************
		if (current_pass == 0)
			compute_next_step(x, y, p);
		else
			data_0[p] = data_1[p]; // The future is now
            sleep(1);
	}
}

void compute_next_step(uint x, uint y, uint p) {
		if ( x > 0 && y > 0 && x < WSX-1 && y < WSY - 1) { // in sandbox
			uint n = 0 ; // Number of living neighbors
			for (uint i = x-1; i <= x+1; i++) {
				for (uint j = y-1; j <= y+1; j++) {
					uint k = i + j * WSX;
					if (k != p && data_0[k] == ALIVE )
						n++;
				}
			}
			if (data_0[p] == DEAD) {
				if ( n >= 1)
					data_1[p] = ALIVE; // Birth
				else
					data_1[p] = DEAD ;
			} 
		}
}