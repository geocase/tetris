#include "tetris.h"

// clang-format off
struct Tetrimino tetrimino_defs[BT_MAX] = {
	{
		.type = BT_I,
		.grid = {
			{0, 1, 0, 0},
			{0, 1, 0, 0},
			{0, 1, 0, 0},
			{0, 1, 0, 0},
		},
	},
	{
		.type = BT_O,
		.grid = {
			{1, 1, 0, 0},
			{1, 1, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
		},
	},
	{
		.type = BT_T,
		.grid = {
			{1, 1, 1, 0},
			{0, 1, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
		},
	},
	{
		.type = BT_S,
		.grid = {
			{0, 1, 1, 0},
			{1, 1, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
		},
	},
	{
		.type = BT_Z,
		.grid = {
			{1, 1, 0, 0},
			{0, 1, 1, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
		},
	},
	{
		.type = BT_J,
		.grid = {
			{0, 1, 0, 0},
			{0, 1, 0, 0},
			{1, 1, 0, 0},
			{0, 0, 0, 0},
		},
	},
	{
		.type = BT_L,
		.grid = {
			{0, 1, 0, 0},
			{0, 1, 0, 0},
			{0, 1, 1, 0},
			{0, 0, 0, 0},
		},
	},
};
// clang-format on

struct Tetris
tetris_Init(float game_time) {
	struct Tetris t = {
	    .score = 0,
	    .level = 0,
	    .color_defs =
	        {color_Normal(82, 82, 82, 255, 255),
	         color_Normal(255, 122, 33, 255, 255),
	         color_Normal(0, 0, 255, 255, 255)},
	    .update_acc    = 0,
	    .speed         = 1.0f, // once per second to start
	    .piece_x       = 3,
	    .piece_y       = 25,
	    .current_piece = tetrimino_Init(BT_L)};
	for(int x = 0; x < PLAYFIELD_X; ++x) {
		for(int y = 0; y < PLAYFIELD_Y; ++y) {
			t.playfield[y][x] = BLOCKCOLOR_EMPTY;
		}
	}
	return t;
}

void
tetris_Update(struct Tetris* to_update, float game_time) {
	to_update->update_acc += game_time;
	printf("%f\n", to_update->update_acc);
	if(to_update->update_acc >= to_update->speed) {
		printf("UPDATE\n");

		tetrimino_Rotate(&(to_update->current_piece));
		to_update->update_acc = 0.0f;
		// to_update->piece_y += 1;
		if(to_update->piece_y + 3 >= PLAYFIELD_Y) {
			to_update->piece_y = 25;
		}
	}
}

struct Tetrimino
tetrimino_Init(unsigned int block_type) {
	return tetrimino_defs[block_type];
}

static void
swap_bool(bool* a, bool* b) {
	bool c = *a;
	*a     = *b;
	*b     = c;
}

void
tetrimino_Rotate(struct Tetrimino* to_rotate) {
	if(to_rotate->type == BT_O) {
		return;
	}

	int edge = 3;
	if(to_rotate->type != BT_I) {
		--edge;
	}
	// hacky and GROSS ICKY BLEH!
	// outer
	swap_bool(&(to_rotate->grid[0][0]), &(to_rotate->grid[0][edge]));
	swap_bool(&(to_rotate->grid[0][0]), &(to_rotate->grid[edge][edge]));
	swap_bool(&(to_rotate->grid[0][0]), &(to_rotate->grid[edge][0]));

	swap_bool(&(to_rotate->grid[0][1]), &(to_rotate->grid[1][edge]));
	swap_bool(&(to_rotate->grid[0][1]), &(to_rotate->grid[edge][edge - 1]));
	swap_bool(&(to_rotate->grid[0][1]), &(to_rotate->grid[edge - 1][0]));

	if(to_rotate->type == BT_I) {
		swap_bool(&(to_rotate->grid[0][2]), &(to_rotate->grid[2][3]));
		swap_bool(&(to_rotate->grid[0][2]), &(to_rotate->grid[3][1]));
		swap_bool(&(to_rotate->grid[0][2]), &(to_rotate->grid[1][0]));

		// inner
		swap_bool(&(to_rotate->grid[1][1]), &(to_rotate->grid[1][2]));
		swap_bool(&(to_rotate->grid[1][1]), &(to_rotate->grid[2][2]));
		swap_bool(&(to_rotate->grid[1][1]), &(to_rotate->grid[2][1]));
	}

	for(int y = 0; y < 4; ++y) {
		for(int x = 0; x < 4; ++x) {
			printf("%d", to_rotate->grid[y][x]);
		}
		printf("\n");
	}
}
