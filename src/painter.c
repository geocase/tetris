#include "painter.h"

void
paint_TetrisPlayfield(struct Renderer render_with, struct Tetris to_render_from) {
	float x_padding  = 100;
	float x_offset   = 2;
	float y_padding  = 40;
	float y_offset   = 2;
	float block_size = 20;

	float border_padding = 10;
	renderer_DrawQuad(
	    render_with,
	    x_padding - border_padding,
	    y_padding - border_padding,
	    border_padding + block_size * PLAYFIELD_X + x_offset * PLAYFIELD_X + (border_padding - x_offset),
	    border_padding + block_size * PLAYFIELD_Y_MIN + y_offset * PLAYFIELD_Y_MIN + (border_padding - y_offset),
	    color_Uniform(0, 0, 0, 255, 255));

	for(int x = 0; x < PLAYFIELD_X; ++x) {
		for(int y = PLAYFIELD_Y_MIN; y < PLAYFIELD_Y; ++y) {
			float x_start_pos = x_padding + (block_size * x) + (x * x_offset);
			float y_start_pos = y_padding + (block_size * (y - 20)) + ((y - 20) * y_offset);
			Color_t to_draw   = to_render_from.color_defs[to_render_from.playfield[x][y]];
			renderer_DrawQuad(render_with, x_start_pos, y_start_pos, block_size, block_size, to_draw);
		}
	}
}