#ifndef MAIN_H
#define MAIN_H

#include "drivers/screen.h"
#include "drivers/kbd.h"
#include "drivers/kbdmap.h"
#include "drivers/kbd.h"
#include "drivers/kbdmap.h"
#include "drivers/screen.h"

#include "kernel/pit.h"
#include "graphics/pomelo.h"

#include "camera.h"
#include "levels.h"
#include "sprites.h"
#include "cat.h"
#include "level.h"
#include "entity.h"

#define SCALE 1.5
#define TILE_SIZE 16

extern int TICKS;
extern Camera camera;
extern Level* cur_level_ptr;
extern KeyEvent kbd_event;
extern u8 kbd_result;

void PROGRAM_CAT_MAIN();

#endif