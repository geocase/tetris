#ifndef TETRIS_H
#define TETRIS_H
#include <inttypes.h>

#define PLAYFIELD_X 10
#define PLAYFIELD_Y 40 // anything above line 20 is hidden

enum BlockColor { BLOCKCOLOR_EMPTY, BLOCKCOLOR_ORANGE, BLOCKCOLOR_BLUE };

struct Tetris {
	uint8_t playfield[PLAYFIELD_X][PLAYFIELD_Y];
	uint32_t score;
	uint32_t level;
};

#endif