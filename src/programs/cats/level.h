#ifndef LEVEL_H
#define LEVEL_H

#include "kernel/util.h"

// Rendering flags
#define RF_HOVER 0x1 // This layer renders on top of entities
#define RF_ANIMATED 0x2 // Animated layer

typedef struct Level {
	u32 width_t;
	u32 height_t;
	u32 tile_size;

	SpriteSheet* spritesheet;

	u8  layer_count;
	s16 **rendering_layers;
	u16 *rendering_flags;
	u8  *colliders;
} Level;

void level_draw_rect(float x, float y, int w, int h, u8 color, int scale);
void level_draw_sprite(SpriteSheet *sheet, int idx, float x, float y, float scale);
void level_draw_sprite_ca(SpriteSheet *sheet, int idx, float x, float y, float scale);
void level_draw_sprite_colored(SpriteSheet *sheet, int idx, float x, float y, float scale, u8 color);
void level_draw_sprite_ca_colored(SpriteSheet *sheet, int idx, float x, float y, float scale, u8 color);
u8 is_tile_collider(Level* level, int x, int y);
u8 is_pos_collider(Level* level, float, float y);

#endif // LEVEL_H
