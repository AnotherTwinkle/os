#include "kernel/util.h"
#include "camera.h"
#include "main.h"

// Quake?
static float Q_rsqrt(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = *(long *)&y;              // evil bit-level hack
    i  = 0x5f3759df - (i >> 1);    // magic constant
    y  = *(float *)&i;
    y  = y * (threehalfs - (x2 * y * y));   // 1st iteration
    // y  = y * (threehalfs - (x2 * y * y)); // optional 2nd iteration

    return y;
}


void camera_update(Camera* camera) {
    float old_x = camera->posx;
    float old_y = camera->posy;

    if (camera->is_following_entity) {

        camera->posx += *(camera->following_dx);
        camera->posy += *(camera->following_dy);

    } else if (camera->is_moving_to_point) {

        float nx = camera->posx + camera->move_dx;
        float ny = camera->posy + camera->move_dy;

        // --- Snap X ---
        if ((camera->move_dx > 0 && nx >= camera->target_x) ||
            (camera->move_dx < 0 && nx <= camera->target_x)) {
            camera->posx = camera->target_x;
        } else {
            camera->posx = nx;
        }

        // --- Snap Y ---
        if ((camera->move_dy > 0 && ny >= camera->target_y) ||
            (camera->move_dy < 0 && ny <= camera->target_y)) {
            camera->posy = camera->target_y;
        } else {
            camera->posy = ny;
        }

        // Stop only when both axes reached
        if (camera->posx == camera->target_x &&
            camera->posy == camera->target_y) {
            camera->is_moving_to_point = 0;
        }
    } else { // Camera is free
        if (kbd_result != 0 && (kbd_event.flags & KBD_FLAG_MAKE)) {
            float camera_speed = 0.2f;
            float zoom_speed = 1;
            if (kbd_event.code == KEY_W) camera->posy -= camera_speed;
            if (kbd_event.code == KEY_S) camera->posy += camera_speed;
            if (kbd_event.code == KEY_A) camera->posx -= camera_speed;
            if (kbd_event.code == KEY_D) camera->posx += camera_speed;

            if (kbd_event.code == KEY_Q) camera->zoom += zoom_speed;
            if (kbd_event.code == KEY_E) camera->zoom -= zoom_speed;
        }
    }

    camera->dx = camera->posx - old_x;
    camera->dy = camera->posy - old_y;
}

void camera_follow_entity(Camera* camera, float* entity_dx, float* entity_dy) {
	camera->is_following_entity = 1;
	camera->following_dx = entity_dx;
	camera->following_dy = entity_dy;
}

void free_camera(Camera* camera) {
	camera->is_following_entity = 0;
}

void camera_move_to(Camera* c, float x, float y, float speed) {
    float dx = x - c->posx;
    float dy = y - c->posy;
    float inv_len = Q_rsqrt(dx*dx + dy*dy);

    if (dx == 0 && dy == 0) {
        c->is_moving_to_point = 0;
        return;
    }

    c->is_moving_to_point = 1;
    c->target_x = x;
    c->target_y = y;
    c->move_dx = dx * inv_len * speed;  
    c->move_dy = dy * inv_len * speed;
}

