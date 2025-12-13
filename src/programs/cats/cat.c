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


void cat_update(Cat* cat) {
	if (cat->state == CAT_WALKING) {
		float speed = 0.03f; // per tick
		float old_posx = cat->posx;
		float old_posy = cat->posy;
		if (cat->orientation == FACING_RIGHT) {
			cat->posx += speed;
		}
		else if (cat->orientation == FACING_LEFT) {
			cat->posx -= speed;
		}

		else if (cat->orientation == FACING_UP) {
			cat->posy -= speed;
		}

		else if (cat->orientation == FACING_DOWN) {
			cat->posy += speed;
		}

		cat->dx = cat->posx - old_posx;
		cat->dy = cat->posy - old_posy;

		if (cat->posx + 1 > cur_level->width_t) {
			cat->orientation = FACING_LEFT;
			set_anim(&cat->anim_state, &anim_walking_left);
		}

		if (cat->posx - 1 < 0) {
			cat->orientation = FACING_RIGHT;
			set_anim(&cat->anim_state, &anim_walking_right);
		}
	}

	if (cat->state == CAT_WALKING && randint(1, 10000) < 10) {
		cat->orientation = FACING_DOWN;
		cat->state = CAT_SITTING_DOWN;
		set_anim(&cat->anim_state, &anim_sitting);
	}

	if (cat->state == CAT_SITTING_DOWN && cat->anim_state.looping == 1) {
		cat->state = CAT_IDLE;
	}

	int chance = randint(0, 1000);
	if (chance < 30 && cat->state == CAT_IDLE && cat->anim_state.looping_for > 5) {
		set_anim(&cat->anim_state, &anim_licking);
		cat->state = CAT_LICKING;
	}

	if (chance < 10 && cat->state == CAT_LICKING && cat->anim_state.looping_for > 20) {
		set_anim(&cat->anim_state, &anim_curled_sleep);
		cat->state = CAT_CURLED;
	}
	update_anim(&cat->anim_state);
}

void draw_cat(Cat *cat, int scale) {
	u32 idx = cat->anim_state.anim->frames[cat->anim_state.frame];
	world_draw_sprite_ca(cat->spritesheet, idx, cat->posx, cat->posy, scale);
}

