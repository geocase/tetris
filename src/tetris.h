#ifndef TETRIS_H
#define TETRIS_H
#include "renderer.h"
#include <inttypes.h>

#define PLAYFIELD_X 10
#define PLAYFIELD_Y 40 // anything above line 20 is hidden
#define PLAYFIELD_Y_MIN 20

enum BlockColor {
	BLOCKCOLOR_EMPTY,
	BLOCKCOLOR_ORANGE,
	BLOCKCOLOR_BLUE,
	BLOCKCOLOR_CYAN,
	BLOCKCOLOR_YELLOW,
	BLOCKCOLOR_PURPLE,
	BLOCKCOLOR_GREEN,
	BLOCKCOLOR_RED,
	BLOCKCOLOR_MAX
};
enum BlockType { BT_I, BT_O, BT_T, BT_S, BT_Z, BT_J, BT_L, BT_MAX, BT_EMPTY };

extern struct Tetrimino tetrimino_defs[BT_MAX];

struct Tetrimino {
	unsigned int type;
	bool grid[4][4];
	int rotation;
	unsigned int block_color;
};

typedef struct {
	bool just_pressed;
	bool is_pressed;
} Keypress_t;

struct Tetris {
	Keypress_t rotate_key;
	Keypress_t left_key;
	Keypress_t right_key;
	Keypress_t hard_drop_key;
	Keypress_t reset_key;
	Keypress_t pause_key;
	Keypress_t fast_drop_key;
	bool hard_dropped;
	uint8_t playfield[PLAYFIELD_Y][PLAYFIELD_X];
	int score;
	int level;

	Color_t color_defs[BLOCKCOLOR_MAX];

	float update_acc;
	float speed;

	int piece_x, piece_y; // top left justified.
	struct Tetrimino current_piece;
	bool game_lost;
	bool paused;

	bool fast_drop_on;

	int lines_cleared;

	int rows_cleared;

	float line_cleared_show_off_acc;
	float line_cleared_show_off_time;
	bool line_show_off;
	int lines_to_show_off[4];

	bool row_cleared[PLAYFIELD_Y_MIN];
};

struct Tetris tetris_Init(float game_time);
void tetris_Update(struct Tetris* to_update, /*hack*/ struct AudioPlayer ap, float game_time);

struct Tetrimino tetrimino_Init();
void tetrimino_Rotate(struct Tetrimino* to_rotate);

#endif