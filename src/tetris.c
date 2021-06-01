#include "tetris.h"

struct Tetris
tetris_Init(float game_time) {
	struct Tetris t = {
	    .score = 0,
	    .level = 0,
	    .color_defs =
	        {color_Uniform(82, 82, 82, 255, 255),
	         color_Uniform(255, 122, 33, 255, 255),
	         color_Uniform(0, 0, 255, 255, 255)},
	    .update_acc = 0,
	    .speed      = 1.0f // once per second to start
	};
	for(int x = 0; x < PLAYFIELD_X; ++x) {
		for(int y = 0; y < PLAYFIELD_Y; ++y) {
			t.playfield[x][y] = BLOCKCOLOR_EMPTY;
		}
	}
	return t;
}

void
tetris_Update(struct Tetris* to_update, float game_time) {
	static int pos_x = 0;
	static int pos_y = 19;

	to_update->update_acc += game_time;
	printf("%f\n", to_update->update_acc);
	if(to_update->update_acc >= to_update->speed) {
		printf("UPDATE\n");
		to_update->update_acc                = 0.0f;
		to_update->playfield[pos_x][pos_y++] = BLOCKCOLOR_EMPTY;
		to_update->playfield[pos_x][pos_y]   = BLOCKCOLOR_ORANGE;
		if(pos_y == PLAYFIELD_Y) {
			pos_y = 19;
			pos_x++;
			if(pos_x == PLAYFIELD_X) {
				pos_x = 0;
			}
		}
	}
}
