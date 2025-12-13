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
