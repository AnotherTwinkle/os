#ifndef MAIN_H
#define MAIN_H

#include "drivers/screen.h"
#include "camera.h"
#include "levels.h"

#define SCALE 1.5
#define TILE_SIZE 16

extern Camera camera;
extern Level* cur_level;

void PROGRAM_CAT_MAIN();

void world_draw_sprite(SpriteSheet *sheet, int idx, float x, float y, float scale);
void world_draw_sprite_ca(SpriteSheet *sheet, int idx, float x, float y, float scale);

#endif