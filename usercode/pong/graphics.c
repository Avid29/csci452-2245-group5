/**
** @file	graphics.h
**
** @author	Adam Dernis
**
** @brief	Graphics library function implementations
*/

#include <common.h>
#include <ulib.h>

#include "graphics.h"

bool_t
clamp(rect *r) {
    bool_t changed = false;

    if (r->pos.x < 0) {
        r->pos.x = 0;
        changed = true;
    }
    else if (r->pos.x > PIXEL_WIDTH) {
        r->pos.x = PIXEL_WIDTH;
        changed = true;
    }
    
    if (r->pos.y < 0) {
        r->pos.y = 0;
        changed = true;
    }
    else if (r->pos.y > PIXEL_HEIGHT) {
        r->pos.y = PIXEL_HEIGHT;
        changed = true;
    }

    // TODO: Clamp size

    return changed;
}

void
draw_rect(void *buf, rect *r, char c) {

    // Clamp the rectangle to fit the pixel space
    clamp(r);

    for (int i = r->pos.y; i <= r->pos.y + r->size.y; i++) {
        umemset(buf + i * PIXEL_WIDTH + r->pos.x, c, r->size.x);
    }
}

void
draw_outline(void *buf, rect *r, char c, int thickness) {
    clamp(r);

    // Hortizontal lines
    umemset(buf + r->pos.y * PIXEL_WIDTH + r->pos.x, c, r->size.x);
    umemset(buf + (r->pos.y + r->size.y-1) * PIXEL_WIDTH + r->pos.x, c, r->size.x);

    // Vertical lines
    for (int i = r->pos.y; i <= r->pos.y + r->size.y; i++) {
        ((char*)buf)[i * PIXEL_WIDTH + r->pos.x] = c;
        ((char*)buf)[i * PIXEL_WIDTH + r->pos.x + r->size.x] = c;
    }

    if (thickness == 1)
        return;

    rect *child = r;
    child->pos.x++;
    child->pos.y++;
    child->size.x-=2;
    child->size.y-=2;
    draw_outline(buf, child, c, thickness-1);
}