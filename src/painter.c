#include "painter.h"
#include <stdio.h>

void
paint_TetrisPlayfield(struct Renderer render_with, struct Tetris to_render_from) {
	// remove later! testing only!
	renderer_DrawTexture(render_with, 0, 0, 800, 600, 0);

	float x_padding      = 100;
	float x_offset       = 2;
	float y_padding      = 40;
	float y_offset       = 2;
	float block_size     = 20;
	float border_padding = 10;

	renderer_DrawQuad(
	    render_with,
	    x_padding - border_padding,
	    y_padding - border_padding,
	    border_padding + block_size * PLAYFIELD_X + x_offset * PLAYFIELD_X + (border_padding - x_offset),
	    border_padding + block_size * PLAYFIELD_Y_MIN + y_offset * PLAYFIELD_Y_MIN + (border_padding - y_offset),
	    color_Normal(0, 0, 0, 255, 255));

	for(int y = PLAYFIELD_Y_MIN; y < PLAYFIELD_Y; ++y) {
		bool is_show_off_line = false;
		for(int i = 0; i < 4; ++i) {
			if(to_render_from.lines_to_show_off[i] == -1) {
				break;
			}
			if(to_render_from.lines_to_show_off[i] + PLAYFIELD_Y_MIN == y) {
				is_show_off_line = true;
				break;
			}
		}
		for(int x = 0; x < PLAYFIELD_X; ++x) {
			float x_start_pos = x_padding + (block_size * x) + (x * x_offset);
			float y_start_pos = y_padding + (block_size * (y - 20)) + ((y - 20) * y_offset);
			Color_t to_draw   = to_render_from.color_defs[to_render_from.playfield[y][x]];
			renderer_DrawQuad(render_with, x_start_pos, y_start_pos, block_size, block_size, to_draw);

			if(is_show_off_line) {
				to_draw = color_Normal(1, 1, 1, 1, 1);
				renderer_DrawQuad(render_with, x_start_pos, y_start_pos, block_size, block_size, to_draw);
			}

			// draw piece
			if(to_render_from.piece_x <= x && to_render_from.piece_x + 3 >= x) {
				if(to_render_from.piece_y <= y && to_render_from.piece_y + 3 >= y) {
					if(to_render_from.current_piece.grid[y - to_render_from.piece_y][x - to_render_from.piece_x]) {
						to_draw = to_render_from.color_defs[to_render_from.current_piece.block_color];
						renderer_DrawQuad(render_with, x_start_pos, y_start_pos, block_size, block_size, to_draw);
					}
				}
			}
		}
	}

	float normal = to_render_from.update_acc / to_render_from.speed;

	renderer_DrawQuad(render_with, 0, 0, normal * render_with.ren_x, 20, color_Normal(1 - normal, 0, normal, 1, 1));

	char levels_cleared_string[20];
	sprintf(&levels_cleared_string, "Lines: %d", to_render_from.lines_cleared);
	renderer_DrawText(render_with, 330, 400 - (48 * 2), 1.0, levels_cleared_string);

	char levels_string[10];
	sprintf(&levels_string, "Level: %d", to_render_from.level);
	renderer_DrawText(render_with, 330, 400 - 48, 1.0, levels_string);

	char current_speed[20];
	sprintf(&current_speed, "Speed: %.2f", to_render_from.speed);
	renderer_DrawText(render_with, 330, 448 - 48, 1.0, current_speed);

	char current_score[20];
	sprintf(&current_score, "Score: %d", to_render_from.score);
	renderer_DrawText(render_with, 330, 448, 1.0, current_score);

	if(to_render_from.game_lost) {
		renderer_DrawTexture(render_with, 0, 0, 800, 600, 1);
	}
}