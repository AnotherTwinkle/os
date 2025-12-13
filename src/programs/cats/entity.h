#ifndef ENTITY_H
#define ENTITY_H

#include "kernel/util.h"

#include "sprites.h"
#include "anim.h"

#define MAX_ENTITIES 	 256
#define ENTITY_SLOT_SIZE 128

extern int active_entity_count;

typedef struct Entity Entity;
typedef void (*update_func)(Entity *);
typedef void (*think_func)(Entity *);

typedef struct Entity {
	float x, y; 
	float vx, vy;

	update_func update;
	think_func think;
	int next_think;

	AnimState anim_state;
	SpriteSheet *spritesheet;
	u8 orientation;

	u16 state;
	u16 substate;

	u32 flags;
	int pool_index;

} Entity;


extern u8 entity_pool[MAX_ENTITIES][ENTITY_SLOT_SIZE];
extern u8 entity_used[MAX_ENTITIES];
extern Entity *active_entities[MAX_ENTITIES];
extern int active_entity_count;


Entity* entity_alloc(int size);
void entity_free(Entity *e); 

void entity_add(Entity *e);
void entity_remove(int index);

void entity_render(Entity *entity, int scale);

#endif // ENTITY_H
