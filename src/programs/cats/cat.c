#include "cat.h"
#include "graphics/pomelo.h"
#include "sprites.h"

#include "drivers/screen.h"
#include "main.h"

// Cat licking itself
u32 licking_frames[] = {19, 20, 21, 22, 23, 21, 22, 23};
u32 licking_durations[] = {30, 20, 20, 20, 20, 15, 15, 15};
Animation anim_licking = {
	.frames = licking_frames,
	.durations = licking_durations,
	.length = 8,
	.loop = 1,
	.loop_start_frame = 5,
};

// Cat gong to sleep (flat)
u32 flat_sleep_frames[] = {24, 25, 26, 27, 27};
u32 flat_sleep_durations[] = {10, 10, 10, 10, 10};
Animation anim_flat_sleep = {
	.frames = flat_sleep_frames,
	.durations = flat_sleep_durations,
	.length = 5,
	.loop = 0,
	.loop_start_frame = -1
};

// Going to sleep (curled)
u32 curled_sleep_frames[] = {19, 28, 29};
u32 curled_sleep_durations[] = {100, 50, 60};
Animation anim_curled_sleep = {
	.frames = curled_sleep_frames,
	.durations = curled_sleep_durations,
	.length = 3,
	.loop = 1,
	.loop_start_frame = 1
};

// Cat sitting down
u32 sitting_frames[] = {16, 17, 18};
u32 sitting_durations[] = {20, 10, 10};
Animation anim_sitting = {
	.frames = sitting_frames,
	.durations = sitting_durations,
	.length = 3,
	.loop = 1,
	.loop_start_frame = 2
};

// Cat walking left (from right)
u32 walking_left_frames[] = {12, 13, 14, 15};
u32 walking_left_durations[] = {12, 12, 12, 12};
Animation anim_walking_left = {
	.frames = walking_left_frames,
	.durations = walking_left_durations,
	.length = 4,
	.loop = 1,
	.loop_start_frame = 0
};


// Cat walking right (from left)
u32 walking_right_frames[] = {4, 5, 6, 7};
u32 walking_right_durations[] = {12, 12, 12, 12};
Animation anim_walking_right = {
	.frames = walking_right_frames,
	.durations = walking_right_durations,
	.length = 4,
	.loop = 1,
	.loop_start_frame = 0
};

// Cat walking up
u32 walking_up_frames[] = {8, 9, 10, 11};
u32 walking_up_durations[] = {12, 12, 12, 12};
Animation anim_walking_up = {
	.frames = walking_up_frames,
	.durations = walking_up_durations,
	.length = 4,
	.loop = 1,
	.loop_start_frame = 0
};

// Cat walking down
u32 walking_down_frames[] = {0, 1, 2, 3};
u32 walking_down_durations[] = {12, 12, 12, 12};
Animation anim_walking_down = {
	.frames = walking_down_frames,
	.durations = walking_down_durations,
	.length = 4,
	.loop = 1,
	.loop_start_frame = 0
};


void cat_walk_update(Entity *e) {
	Cat *cat = (Cat *)e;
	float speed = 0.03f; // per tick
	float old_x = cat->base.x;
	float old_y = cat->base.y;
	if (cat->base.orientation == FACING_RIGHT) {
		cat->base.x += speed;
	}
	else if (cat->base.orientation == FACING_LEFT) {
		cat->base.x -= speed;
	}

	else if (cat->base.orientation == FACING_UP) {
		cat->base.y -= speed;
	}

	else if (cat->base.orientation == FACING_DOWN) {
		cat->base.y += speed;
	}

	cat->dx = cat->base.x - old_x;
	cat->dy = cat->base.y - old_y;

	update_anim(&cat->base.anim_state);
}

void cat_walk_think(Entity *e) {
	Cat *cat = (Cat *)e;
	if (cat->base.x + 1 > cur_level_ptr->width_t ||
		is_pos_collider(cur_level_ptr, cat->base.x + 1, cat->base.y)) {
		cat->base.orientation = FACING_LEFT;
		set_anim(&cat->base.anim_state, &anim_walking_left);
	}

	if (cat->base.x - 1 < 0 ||
		is_pos_collider(cur_level_ptr, cat->base.x - 1, cat->base.y)) {
		cat->base.orientation = FACING_RIGHT;
		set_anim(&cat->base.anim_state, &anim_walking_right);
	}

	if (cat->base.y + 1 > cur_level_ptr->height_t ||
		is_pos_collider(cur_level_ptr, cat->base.x, cat->base.y + 1)) {
		cat->base.orientation = FACING_UP;
		set_anim(&cat->base.anim_state, &anim_walking_up);
	}

	if (cat->base.y - 1 < 0 ||
		is_pos_collider(cur_level_ptr, cat->base.x, cat->base.y - 1)) {
		cat->base.orientation = FACING_DOWN;
		set_anim(&cat->base.anim_state, &anim_walking_down);
	}

	if (randint(0, 100) < 1) {
		cat->base.think = cat_idle_think;
		cat->base.update = cat_idle_update;
		cat->base.next_think = TICKS + 20;
		set_anim(&cat->base.anim_state, &anim_sitting);
	}

	cat->base.next_think = TICKS + 10;
}

void cat_idle_think(Entity *e) {
	e->next_think = TICKS + 10;

	int chance = randint(1, 100);
	if (chance < 30 && e->anim_state.anim == &anim_sitting && e->anim_state.looping_for > 5) {
		set_anim(&e->anim_state, &anim_licking);
	}

	if (chance < 10 && e->anim_state.anim == &anim_licking && e->anim_state.looping_for > 20) {
		set_anim(&e->anim_state, &anim_curled_sleep);
	}
}

void cat_idle_update(Entity *e) {
	update_anim(&e->anim_state);
}
