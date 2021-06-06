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
		.rotate_key = {
			.is_pressed = false,
			.just_pressed  = false,
		},
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

static bool
tetris_MoveOkay(struct Tetris* to_check, int x_check, int y_check) {
	for(int y = 0; y < 4; y++) {
		for(int x = 0; x < 4; ++x) {
			if(to_check->current_piece.grid[y][x]) {
				if(y + to_check->piece_y + y_check >= PLAYFIELD_Y || y + to_check->piece_y + y_check < 0) {
				return false;
				}
				if(x + to_check->piece_x + x_check >= PLAYFIELD_X || x + to_check->piece_x + x_check < 0) {
					return false;
				}
				if(to_check->playfield[y + to_check->piece_y + y_check][x + to_check->piece_x + x_check] != BLOCKCOLOR_EMPTY) {
					return false;
				}
			}
		}
	}
	return true;
}

static bool
tetris_StampPiece(struct Tetris* to_be_stamped_on) {
	for(int y = 0; y < 4; y++) {
		for(int x = 0; x < 4; ++x) {
			if(to_be_stamped_on->current_piece.grid[y][x]) {
				to_be_stamped_on->playfield[y + to_be_stamped_on->piece_y][x + to_be_stamped_on->piece_x] = BLOCKCOLOR_BLUE;
			}
		}
	}
	return true;
}

void
tetris_Update(struct Tetris* to_update, float game_time) {
	// for(int x = 0; x < PLAYFIELD_X; ++x) {
	// 	to_update->playfield[39][x] = BLOCKCOLOR_BLUE;
	// }
	if(to_update->rotate_key.just_pressed) {
		if(!to_update->rotate_key.is_pressed) {
			tetrimino_Rotate(&(to_update->current_piece));
			if(!tetris_MoveOkay(to_update, 0, 0)) {
				tetrimino_Rotate(&(to_update->current_piece));
				tetrimino_Rotate(&(to_update->current_piece));
				tetrimino_Rotate(&(to_update->current_piece));

			}
			to_update->rotate_key.is_pressed = true;
		}
	} else {
		to_update->rotate_key.is_pressed = false;
	}

	if(to_update->left_key.just_pressed) {
		if(!to_update->left_key.is_pressed) {
			if(tetris_MoveOkay(to_update, -1, 0)) {
				to_update->piece_x -= 1;
			}
			to_update->left_key.is_pressed = true;
		}
	} else {
		to_update->left_key.is_pressed = false;
	}

	if(to_update->right_key.just_pressed) {
		if(!to_update->right_key.is_pressed) {
			if(tetris_MoveOkay(to_update, 1, 0)) {
				to_update->piece_x += 1;
			}
			to_update->right_key.is_pressed = true;
		}
	} else {
		to_update->right_key.is_pressed = false;
	}

	to_update->update_acc += game_time;
	if(to_update->update_acc >= to_update->speed) {
		printf("UPDATE\n");

		to_update->update_acc = 0.0f;
		if(tetris_MoveOkay(to_update, 0, 1)) {
			to_update->piece_y += 1;
		} else {
			tetris_StampPiece(to_update);
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
