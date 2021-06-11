#include "tetris.h"
#include "audio.h"
#include <stdio.h>
#include <stdlib.h>

// clang-format off
struct Tetrimino tetrimino_defs[BT_MAX] = {
	{
		.type = BT_I,
		.grid = {
			{0, 0, 0, 0},
			{1, 1, 1, 1},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
		},
		.block_color = BLOCKCOLOR_CYAN,
	},
	{
		.type = BT_O,
		.grid = {
			{1, 1, 0, 0},
			{1, 1, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
		},
		.block_color = BLOCKCOLOR_YELLOW,
	},
	{
		.type = BT_T,
		.grid = {
			{1, 1, 1, 0},
			{0, 1, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
		},
		.block_color = BLOCKCOLOR_PURPLE,
	},
	{
		.type = BT_S,
		.grid = {
			{0, 1, 1, 0},
			{1, 1, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
		},
		.block_color = BLOCKCOLOR_GREEN,
	},
	{
		.type = BT_Z,
		.grid = {
			{1, 1, 0, 0},
			{0, 1, 1, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
		},
		.block_color = BLOCKCOLOR_RED,
	},
	{
		.type = BT_J,
		.grid = {
			{1, 0, 0, 0},
			{1, 1, 1, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
		},
		.block_color = BLOCKCOLOR_BLUE,
	},
	{
		.type = BT_L,
		.grid = {
			{0, 0, 1, 0},
			{1, 1, 1, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
		},
		.block_color = BLOCKCOLOR_ORANGE,
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
	         color_Normal(0, 0, 255, 255, 255),
	         color_Normal(66, 233, 245, 255, 255),
	         color_Normal(245, 239, 66, 255, 255),
	         color_Normal(227, 66, 245, 255, 255),
	         color_Normal(66, 245, 96, 255, 255),
	         color_Normal(255, 0, 0, 255, 255)},
	    .update_acc    = 0,
	    .speed         = .75f, // once per second to start
	    .piece_x       = 3,
	    .piece_y       = 20,
	    .current_piece = tetrimino_Init(rand() % BT_MAX),
	    .game_lost     = false,
	    .lines_cleared = 0,
	    .level         = 1,
	    .paused        = false,
	    .fast_drop_on  = false,
		.line_cleared_show_off_time = 5.0f,
		.line_show_off = false};
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
				if(to_check->playfield[y + to_check->piece_y + y_check][x + to_check->piece_x + x_check] !=
				   BLOCKCOLOR_EMPTY) {
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
				to_be_stamped_on->playfield[y + to_be_stamped_on->piece_y][x + to_be_stamped_on->piece_x] =
				    to_be_stamped_on->current_piece.block_color;
			}
		}
	}
	return true;
}

static void
tetris_LevelUp(struct Tetris* to_level_up) {
	to_level_up->level += 1;
	to_level_up->speed -= (to_level_up->speed / 4.0f);
}

void
tetris_Update(struct Tetris* to_update, /*hack*/ struct AudioPlayer ap, float game_time) {
	if(to_update->line_show_off) {
		to_update->line_cleared_show_off_acc += game_time;
		if(to_update->line_cleared_show_off_acc > to_update->line_cleared_show_off_time) {
		} else {
			printf("SHOW OFF %f\n", to_update->line_cleared_show_off_acc);
			return;
		}
	}


	float volume = .01f;
	if(to_update->reset_key.just_pressed) {
		if(!to_update->reset_key.is_pressed) {
			*to_update = tetris_Init(0);
			audioplayer_PlaySoundWithVolume(ap, 1, volume, false, false);

			return;
			to_update->reset_key.is_pressed = true;
		}
	} else {
		to_update->reset_key.is_pressed = false;
	}

	if(to_update->pause_key.just_pressed) {
		if(!to_update->pause_key.is_pressed) {
			to_update->paused               = !to_update->paused;
			to_update->pause_key.is_pressed = true;
		}
	} else {
		to_update->pause_key.is_pressed = false;
	}

	if(to_update->game_lost) {
		return;
	}

	if(to_update->paused) {
		return;
	}

	if(to_update->rotate_key.just_pressed) {
		if(!to_update->rotate_key.is_pressed) {
			tetrimino_Rotate(&(to_update->current_piece));
			if(!tetris_MoveOkay(to_update, 0, 0)) {
				audioplayer_PlaySoundWithVolume(ap, 0, volume, false, false);

				tetrimino_Rotate(&(to_update->current_piece));
				tetrimino_Rotate(&(to_update->current_piece));
				tetrimino_Rotate(&(to_update->current_piece));
			}
			to_update->rotate_key.is_pressed = true;
		}
	} else {
		to_update->rotate_key.is_pressed = false;
	}

	if(to_update->hard_drop_key.just_pressed) {
		if(!to_update->hard_drop_key.is_pressed) {
			while(tetris_MoveOkay(to_update, 0, 1)) {
				to_update->piece_y++;
			}
			to_update->hard_dropped             = true;
			to_update->hard_drop_key.is_pressed = true;
			audioplayer_PlaySoundWithVolume(ap, 0, volume, false, false);
		}
	} else {
		to_update->hard_drop_key.is_pressed = false;
	}

	if(to_update->left_key.just_pressed) {
		if(!to_update->left_key.is_pressed) {
			if(tetris_MoveOkay(to_update, -1, 0)) {
				to_update->piece_x -= 1;
			} else {
				audioplayer_PlaySoundWithVolume(ap, 0, volume, false, false);
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
			} else {
				audioplayer_PlaySoundWithVolume(ap, 0, volume, false, false);
			}
			to_update->right_key.is_pressed = true;
		}
	} else {
		to_update->right_key.is_pressed = false;
	}

	to_update->fast_drop_on = to_update->fast_drop_key.is_pressed;

	to_update->update_acc += game_time;
	float adjusted_speed = to_update->speed;
	if(to_update->fast_drop_on) {
		adjusted_speed /= 10.0f;
	}
	if(to_update->update_acc >= adjusted_speed || to_update->hard_dropped) {
		to_update->update_acc = 0.0f;
		if(tetris_MoveOkay(to_update, 0, 1)) {
			to_update->piece_y += 1;
		} else {
			tetris_StampPiece(to_update);
			to_update->piece_y       = 20;
			to_update->piece_x       = 3;
			to_update->current_piece = tetrimino_Init(rand() % BT_MAX);
			if(!(tetris_MoveOkay(to_update, 0, 0))) {
				to_update->game_lost = true;
			}
		}
		to_update->hard_dropped = false;
	}

	bool row_cleared[PLAYFIELD_Y_MIN];
	for(int i = 0; i < PLAYFIELD_Y_MIN; ++i) {
		row_cleared[i] = true;
	}

	// check board
	for(int y = PLAYFIELD_Y_MIN; y < PLAYFIELD_Y; ++y) {
		for(int x = 0; x < PLAYFIELD_X; ++x) {
			if(to_update->playfield[y][x] == BLOCKCOLOR_EMPTY) {
				row_cleared[y - PLAYFIELD_Y_MIN] = false;
				break;
			}
		}
	}

	// clear lines to show off
	for(int i = 0; i < 4; ++i) {
		to_update->lines_to_show_off[i] = -1;
	}

	int rows_cleared = 0;
	for(int i = 0; i < PLAYFIELD_Y_MIN; ++i) {
		if(row_cleared[i]) {
			to_update->lines_to_show_off[rows_cleared] = i;
			++rows_cleared;
			for(int x = 0; x < PLAYFIELD_X; ++x) {
				to_update->playfield[i + PLAYFIELD_Y_MIN][x] = BLOCKCOLOR_EMPTY;
			}
			for(int above = i; above > 0; above--) {
				for(int above_x = 0; above_x < PLAYFIELD_X; ++above_x) {
					to_update->playfield[above + PLAYFIELD_Y_MIN][above_x] =
					    to_update->playfield[above - 1 + PLAYFIELD_Y_MIN][above_x];
				}
			}
		}
	}

	if(rows_cleared) {
		to_update->line_show_off = true;
		to_update->line_cleared_show_off_acc = 0.0f;
	}

	

	int lines_update = 0;
	int score_update = 0;
	switch(rows_cleared) {
	case 1:
		lines_update = 1;
		score_update = 40 * (to_update->level + 1);
		break;
	case 2:
		lines_update = 3;
		score_update = 100 * (to_update->level + 1);
		break;
	case 3:
		lines_update = 5;
		score_update = 300 * (to_update->level + 1);
		break;
	case 4:
		lines_update = 8;
		score_update = 1200 * (to_update->level + 1);
		break;
	default:
		lines_update = 0;
		score_update = 0;

		break;
	}
	to_update->lines_cleared += lines_update;
	to_update->score += score_update;

	if(to_update->lines_cleared >= to_update->level * 5) {
		tetris_LevelUp(to_update);
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
	to_rotate->rotation++;
	to_rotate->rotation %= 4;
}
