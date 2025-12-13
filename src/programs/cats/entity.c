#include "entity.h"

#include "kernel/util.h"
#include "graphics/pomelo.h"
#include "drivers/screen.h"

#include "main.h"

u8 entity_pool[MAX_ENTITIES][ENTITY_SLOT_SIZE];
u8 entity_used[MAX_ENTITIES];
Entity *active_entities[MAX_ENTITIES];
int active_entity_count = 0;

Entity *entity_alloc(int size) {
	if (size > ENTITY_SLOT_SIZE) {
		while(1); // Remember to replace with panic function
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (!entity_used[i]) {
			entity_used[i] = 1;
			memset(entity_pool[i], 0, ENTITY_SLOT_SIZE);
			Entity *e =  (Entity*)entity_pool[i];
			e->pool_index = i;

			return e;
		}
	}

	while(1); // Panic...
}

void entity_free(Entity *e) {
	entity_used[e->pool_index] = 0;
}

void entity_add(Entity *e) {
	active_entities[active_entity_count++] = e;
}

void entity_remove(int index) {
	active_entities[index] = active_entities[--active_entity_count];
}

void entity_render(Entity *entity, int scale) {
	u32 idx = entity->anim_state.anim->frames[entity->anim_state.frame];
	level_draw_sprite_ca(entity->spritesheet, idx, entity->x, entity->y, scale);
}