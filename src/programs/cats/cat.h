#ifndef CAT_H
#define CAT_H

#include "kernel/util.h"
#include "kernel/pit.h"

#include "graphics/pomelo.h"
#include "entity.h"
#include "sprites.h"
#include "anim.h"

#define CAT_IDLE 0
#define CAT_WALKING 1
#define CAT_LICKING 2
#define CAT_SLEEPING 3
#define CAT_CURLED 4
#define CAT_SITTING_DOWN 5

typedef struct Cat {
	Entity base;
	float dx, dy; // Amount moved last tick
} Cat;

// Defined animations
extern Animation anim_licking;
extern Animation anim_flat_sleep;
extern Animation anim_curled_sleep;
extern Animation anim_sitting;
extern Animation anim_walking_left;
extern Animation anim_walking_right;
extern Animation anim_walking_up;
extern Animation anim_walking_down;

void cat_walk_update(Entity* e);
void cat_walk_think(Entity* e);

void cat_idle_update(Entity* e);
void cat_idle_think(Entity* e);

#endif