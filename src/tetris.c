#include "tetris.h"

struct Tetris
tetris_Init() {
    struct Tetris t = {
        .score = 0,
        .level = 0,
        .color_defs = {
            color_Uniform(82, 82, 82, 255, 255),
            color_Uniform(255, 122, 33, 255, 255),
			color_Uniform(0, 0, 255, 255, 255)
        }
    };
    for(int x = 0; x < PLAYFIELD_X; ++x) {
		for(int y = 0; y < PLAYFIELD_Y; ++y) {
			t.playfield[x][y] = BLOCKCOLOR_EMPTY;
		}
	}

    return t;
}
