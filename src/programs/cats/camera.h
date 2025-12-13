#ifndef CAMERA_H
#define CAMERA_H

#include "kernel/util.h"

typedef struct Camera {
	float posx;
	float posy;

	float dx;
	float dy;

	int zoom;

	int is_following_entity;
	float* following_dx;
	float* following_dy;

	int is_moving_to_point;
	float target_x;
	float target_y;
	float move_dx;
	float move_dy;
} Camera;



void camera_update(Camera* camera);
void camera_follow_entity(Camera* camera, float* entity_dx, float* entity_dy);
void free_camera(Camera* camera);
void camera_move_to(Camera*, float x, float y, float speed);

#endif // CAMERA_H
