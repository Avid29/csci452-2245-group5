/**
** @file	graphics.c
**
** @author	Adam Dernis
**
** @brief	Graphics library function implementations
*/

#include <common.h>

#include "ugraphics.h"

bool_t
clamp(rect_t *r) {
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
draw_rect(void *buf, rect_t r, char c) {

    // Clamp the rectangle to fit the pixel space
    // Never ended up implementing clamping
    // clamp(&r);

    // Draw the rectangle by line
    for (int i = r.pos.y; i <= r.pos.y + r.size.y; i++) {
        umemset(buf + i * PIXEL_WIDTH + r.pos.x, r.size.x, c);
    }
}

void
draw_outline(void *buf, rect_t r, char c, int thickness) {
    // Never ended up implementing clamping
    // clamp(&r);

    // Hortizontal lines
    umemset(buf + r.pos.y * PIXEL_WIDTH + r.pos.x, r.size.x, c);
    umemset(buf + (r.pos.y + r.size.y-1) * PIXEL_WIDTH + r.pos.x, r.size.x, c);

    // Vertical lines
    for (int i = r.pos.y; i <= (r.pos.y + r.size.y)-1; i++) {
        ((char*)buf)[i * PIXEL_WIDTH + r.pos.x] = c;
        ((char*)buf)[i * PIXEL_WIDTH + r.pos.x + r.size.x] = c;
    }

    if (thickness == 1)
        return;

    // Recurse for thickness
    rect_t child = r;
    child.pos.x++;
    child.pos.y++;
    child.size.x-=2;
    child.size.y-=2;
    draw_outline(buf, child, c, thickness-1);
}
