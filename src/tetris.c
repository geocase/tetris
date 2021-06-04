#include "tetris.h"

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
	    .current_piece = tetrimino_Init()};
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
		to_update->update_acc = 0.0f;
		to_update->piece_y += 1;
		if(to_update->piece_y + 3 >= PLAYFIELD_Y) {
			to_update->piece_y = 25;
		}
	}
}

struct Tetrimino
tetrimino_Init() {
	struct Tetrimino t = {
	    .type = BT_I, .grid = {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}, .rotation = 0};
	return t;
}

void
tetrimino_Rotate(struct Tetrimino* to_rotate) {
	struct Tetrimino temp = {
	    .type = to_rotate->type, .grid = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, .rotation = 0};
}
