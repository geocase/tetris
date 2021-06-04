#include "painter.h"

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
		for(int x = 0; x < PLAYFIELD_X; ++x) {
			float x_start_pos = x_padding + (block_size * x) + (x * x_offset);
			float y_start_pos = y_padding + (block_size * (y - 20)) + ((y - 20) * y_offset);
			Color_t to_draw   = to_render_from.color_defs[to_render_from.playfield[y][x]];
			renderer_DrawQuad(render_with, x_start_pos, y_start_pos, block_size, block_size, to_draw);

			// draw piece
			if(to_render_from.piece_x <= x && to_render_from.piece_x + 3 >= x) {
				if(to_render_from.piece_y <= y && to_render_from.piece_y + 3 >= y) {
					if(to_render_from.current_piece.grid[y - to_render_from.piece_y][x - to_render_from.piece_x]) {
						to_draw = color_Normal(255, 0, 0, 255, 255);
						renderer_DrawQuad(render_with, x_start_pos, y_start_pos, block_size, block_size, to_draw);
					}
				}
			}
		}
	}

	float normal = to_render_from.update_acc / to_render_from.speed;

	renderer_DrawQuad(render_with, 0, 0, normal * render_with.ren_x, 20, color_Normal(1 - normal, 0, normal, 1, 1));
}