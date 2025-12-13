#include "kernel/util.h"
#include "graphics/pomelo.h"

#include "main.h"
#include "level.h"

void level_draw_rect(float x, float y, int w, int h, u8 color, int scale) {
	int screen_x  = (int)roundf((x - camera.posx) * TILE_SIZE * scale);
	int screen_y  = (int)roundf((y - camera.posy) * TILE_SIZE * scale);
	if (screen_x > SCREEN_WIDTH || screen_y > SCREEN_HEIGHT || screen_x+w < 0 || screen_y+h < 0) return;

	pml_draw_rect(screen_x, screen_y, w, h, color);
}

void level_draw_sprite(SpriteSheet *sheet, int idx, float x, float y, float scale) {
	int screen_x  = (int)roundf((x - camera.posx) * TILE_SIZE * scale);
	int screen_y  = (int)roundf((y - camera.posy) * TILE_SIZE * scale);
	if (screen_x > SCREEN_WIDTH || screen_y > SCREEN_HEIGHT || screen_x+sheet->unit_width < 0 || screen_y+sheet->unit_height < 0) return;

	pml_draw_sprite(sheet, idx, screen_x, screen_y, scale);
}

void level_draw_sprite_colored(SpriteSheet *sheet, int idx, float x, float y, float scale, u8 color) {
	int screen_x  = (int)roundf((x - camera.posx) * TILE_SIZE * scale);
	int screen_y  = (int)roundf((y - camera.posy) * TILE_SIZE * scale);
	if (screen_x > SCREEN_WIDTH || screen_y > SCREEN_HEIGHT || screen_x+sheet->unit_width < 0 || screen_y+sheet->unit_height < 0) return;

	pml_draw_sprite_colored(sheet, idx, screen_x, screen_y, scale, color);
}

void level_draw_sprite_ca(SpriteSheet *sheet, int idx, float x, float y, float scale) {
	int screen_x  = (int)roundf((x - camera.posx) * TILE_SIZE * scale);
	int screen_y  = (int)roundf((y - camera.posy) * TILE_SIZE * scale);
	int unit_width = sheet->unit_width;
	int unit_height = sheet->unit_height;

	int adj_x = (unit_width * scale) / 2;
    int adj_y = (unit_height * scale) / 2;

	if ((screen_x - adj_x > SCREEN_WIDTH) ||
		(screen_y - adj_y > SCREEN_HEIGHT) ||
		(screen_x + adj_x < 0) ||
		(screen_y + adj_y < 0)) {
		return;
	}
	pml_draw_sprite_ca(sheet, idx, screen_x, screen_y, scale);
}

void level_draw_sprite_ca_colored(SpriteSheet *sheet, int idx, float x, float y, float scale, u8 color) {
	int screen_x  = (int)roundf((x - camera.posx) * TILE_SIZE * scale);
	int screen_y  = (int)roundf((y - camera.posy) * TILE_SIZE * scale);
	int unit_width = sheet->unit_width;
	int unit_height = sheet->unit_height;

	int adj_x = (unit_width * scale) / 2;
    int adj_y = (unit_height * scale) / 2;

	if ((screen_x - adj_x > SCREEN_WIDTH) ||
		(screen_y - adj_y > SCREEN_HEIGHT) ||
		(screen_x + adj_x < 0) ||
		(screen_y + adj_y < 0)) {
		return;
	}
	pml_draw_sprite_ca_colored(sheet, idx, screen_x, screen_y, scale, color);
}

u8 is_tile_collider(Level* level, int x, int y) {
	if (level->colliders[x * level->height_t + y]) return 1;
	return 0;
}

u8 is_pos_collider(Level* level, float x, float y) {
	return is_tile_collider(level, (int)x, (int)y);
}