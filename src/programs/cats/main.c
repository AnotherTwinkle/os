#include "kernel/util.h"
#include "kernel/pit.h"

#include "drivers/kbd.h"
#include "drivers/kbdmap.h"
#include "drivers/screen.h"

#include "graphics/pomelo.h"

#include "sprites.h"
#include "cat.h"
#include "main.h"

static u32 SEED  = 0xFACEDAB;

static u8 arr_cur_buffer[SCREEN_SIZE];
static u8 arr_next_buffer[SCREEN_SIZE];

static u8 *cur_buffer = arr_cur_buffer;
static u8 *next_buffer = arr_next_buffer;

static int KBD_SUB_ID;

Level* cur_level = &level0;

Camera camera = {
	.posx = 0.0f,
	.posy = 0.0f,
	
	.dx = 0.0f,
	.dy = 0.0f,

	.zoom = 1.0f,

	.is_following_entity = 0,
	.following_dx = 0x0,
	.following_dy = 0x0,

	.is_moving_to_point = 0,
	.target_x = 0.0f,
	.target_y = 0.0f,
	.move_dx = 0.0f,
	.move_dy = 0.0f
};

void world_draw_sprite(SpriteSheet *sheet, int idx, float x, float y, float scale) {
	int screen_x  = (int)roundf((x - camera.posx) * TILE_SIZE * scale);
	int screen_y  = (int)roundf((y - camera.posy) * TILE_SIZE * scale);
	pml_draw_sprite(sheet, idx, screen_x, screen_y, roundf(scale));
}

void world_draw_sprite_ca(SpriteSheet *sheet, int idx, float x, float y, float scale) {
	int screen_x  = (int)roundf((x - camera.posx) * TILE_SIZE * scale);
	int screen_y  = (int)roundf((y - camera.posy) * TILE_SIZE * scale);
	pml_draw_sprite_ca(sheet, idx, screen_x, screen_y, roundf(scale));
}


static void SWAP() {
	u8 *tmp = cur_buffer;
	cur_buffer = next_buffer;
	next_buffer = tmp;

	pml_setbuffer(next_buffer, SCREEN_WIDTH, SCREEN_HEIGHT);

	memcpy(SCREEN_ADDR, cur_buffer, SCREEN_SIZE);
}

void PROGRAM_CAT_MAIN() {
	srand(SEED + get_ticks());

	KBD_SUB_ID = kbd_queue_subscribe(&kbd_queue);
	KeyEvent event;

	pml_setbuffer(next_buffer, SCREEN_WIDTH, SCREEN_HEIGHT);

	init_sprites();

	AnimState cat_anim_state;
	SpriteSheet* cat_sprites[4] = {&cat0_sprites, &cat1_sprites,
								   &cat2_sprites, &cat3_sprites};
	Cat cats[4];
	for (int i = 0; i < 4; i++) {
		cats[i].posx = 3.125f;
		cats[i].posy = 3 + i;
		cats[i].dx = 0;
		cats[i].dy = 0;
		cats[i].type = 0;
		cats[i].state = CAT_WALKING;
		cats[i].orientation = (i % 2 ? FACING_LEFT : FACING_RIGHT);
		cats[i].spritesheet = cat_sprites[i];

		cats[i].anim_state = cat_anim_state;
		set_anim(&cats[i].anim_state, 
			(cats[i].orientation == FACING_LEFT) ?
			&anim_walking_left :
			&anim_walking_right
			);
	}

	// camera_follow_entity(&camera, &cats[0].dx, &cats[0].dy);
	// camera_move_to(&camera, 4, 2, 0.02f);
	while(1) {
		// Keyboard
		u8 kbd_result = kbd_dequeue(&kbd_queue, KBD_SUB_ID, &event);
		if (kbd_result != 0 && (event.flags & KBD_FLAG_MAKE)) {
			float camera_speed = 0.2f;
			float zoom_speed = 1;
			if (event.code == KEY_W) camera.posy -= camera_speed;
			if (event.code == KEY_S) camera.posy += camera_speed;
			if (event.code == KEY_A) camera.posx -= camera_speed;
			if (event.code == KEY_D) camera.posx += camera_speed;

			if (event.code == KEY_Q) camera.zoom += zoom_speed;
			if (event.code == KEY_E) camera.zoom -= zoom_speed;
		}

		// Update
		for (int i = 0; i < 4; i++) {
			cat_update(&cats[i]);
		}

		// RENDERING
		pml_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xff);
		for (int x = 0; x < cur_level->width_t; x++) {
			for (int y = 0; y < cur_level->height_t; y++) {
				for (int l = 0; l < cur_level->layer_count; l++) {
					s16 sprite = cur_level->rendering_layers[l][x*cur_level->height_t + y];
					if (sprite < 0) continue;
					world_draw_sprite(cur_level->spritesheet, sprite, x, y, camera.zoom);
				}
				// world_draw_sprite(cur_level->spritesheet, 16, x, y, SCALE);
				// pml_draw_rect(x, y, 16*);
			}
		}

		// int x[] = {5, 8, 5, 4, 13, 3};
		// int y[] = {18, 0, 13, 1, 5, 6};
		// for (int i = 0; i < 6; i++ ) {
		// 	pml_draw_sprite(&level0_sprites, (i%2)+1, x[i]*16, y[i]*16, SCALE);
		// }

		for (int i = 0; i < 4; i++) {
			draw_cat(&cats[i], camera.zoom);	
		}
		

		for (int i = 0; i < cats[0].anim_state.frame; i++) {
			pml_draw_rect(4*i, 0, 2, 2, 0xff);
		}

		for (int i = 0; i < cats[0].anim_state.anim->length; i++) {
			pml_draw_rect(4*i, 8, 2, 2, 0xff);
		}

		for (int i = 0; i < cats[0].state ; i++) {
			pml_draw_rect(4*i, 16, 2, 2, 0xff);
		}

		for (int i = 0; i < cats[0].anim_state.looping_for ; i++) {
			pml_draw_rect(4*i, 24, 2, 2, 0xff);
		}

		for (int i = 0; i < (int)cats[0].posx; i++) {
			pml_draw_rect(4*i, 32, 2, 2, 0xff);
		}


		for (int i = 0; i < (int)cats[0].posy; i++) {
			pml_draw_rect(4*i, 40, 2, 2, 0xff);
		}

		update_camera(&camera);
		sleep(10);
		SWAP();
	}
}
