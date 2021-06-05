#ifndef TETRIS_H
#define TETRIS_H
#include "renderer.h"
#include <inttypes.h>

#define PLAYFIELD_X 10
#define PLAYFIELD_Y 40 // anything above line 20 is hidden
#define PLAYFIELD_Y_MIN 20

enum BlockColor { BLOCKCOLOR_EMPTY, BLOCKCOLOR_ORANGE, BLOCKCOLOR_BLUE, BLOCKCOLOR_MAX };
enum BlockType { BT_I, BT_O, BT_T, BT_S, BT_Z, BT_J, BT_L, BT_MAX, BT_EMPTY };


extern struct Tetrimino tetrimino_defs[BT_MAX];

struct Tetrimino {
	unsigned int type;
	bool grid[4][4];
	int rotation;
};

struct Tetris {
	uint8_t playfield[PLAYFIELD_Y][PLAYFIELD_X];
	uint32_t score;
	uint32_t level;

	Color_t color_defs[BLOCKCOLOR_MAX];

	float update_acc;
	float speed;

	int piece_x, piece_y; // top left justified.
	struct Tetrimino current_piece;
};

struct Tetris tetris_Init(float game_time);
void tetris_Update(struct Tetris* to_update, float game_time);

struct Tetrimino tetrimino_Init();
void tetrimino_Rotate(struct Tetrimino* to_rotate);

#endif