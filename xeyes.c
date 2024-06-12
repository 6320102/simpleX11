/*
 * xeyes.c - a follow the mouse program
 *
 * originally written by 
 *    Jeremy Huxtable
 * modified for X11 by 
 *    Stuart W. Marks
 * modified for X11R4 by 
 *    Keith Packard
 * modified for ANSI C and other minor improvements by 
 *    Michael D. Berger
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <math.h>

#define MIN_WIDTH 300
#define MIN_HEIGHT 300

Display *dpy;
int screen;
Window win;
GC gc;
XGCValues gcvals;
Colormap cmap;
XColor white, black, red;

void draw_eye(int x, int y, int w, int h, int mx, int my) {
    double dist, angle;
    int pupil_x, pupil_y;

    XSetForeground(dpy, gc, black.pixel);
    XFillArc(dpy, win, gc, x, y, w, h, 0, 360 * 64);

    dist = hypot(mx - (x + w / 2), my - (y + h / 2));
    angle = atan2(my - (y + h / 2), mx - (x + w / 2));
    if (dist > w / 4) {
        dist = w / 4;
    }

    pupil_x = (int)(x + w / 2 + cos(angle) * dist - w / 8);
    pupil_y = (int)(y + h / 2 + sin(angle) * dist - h / 8);

    XSetForeground(dpy, gc, white.pixel);
    XFillArc(dpy, win, gc, x + w / 8, y + h / 8, w * 3 / 4, h * 3 / 4, 0, 360 * 64);

    XSetForeground(dpy, gc, black.pixel);
    XFillArc(dpy, win, gc, pupil_x, pupil_y, w / 4, h / 4, 0, 360 * 64);
}

int main(int argc, char *argv[]) {
    XEvent event;
    int width = MIN_WIDTH, height = MIN_HEIGHT;
    int mouse_x = 0, mouse_y = 0;
    int eye_width, eye_height;

    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        fprintf(stderr, "Unable to open display\n");
        return 1;
    }

    screen = DefaultScreen(dpy);
    cmap = DefaultColormap(dpy, screen);
    XAllocNamedColor(dpy, cmap, "white", &white, &white);
    XAllocNamedColor(dpy, cmap, "black", &black, &black);

    win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 0, 0, width, height, 1, black.pixel, white.pixel);
    XSelectInput(dpy, win, ExposureMask | KeyPressMask | PointerMotionMask);
    XMapWindow(dpy, win);

    gc = XCreateGC(dpy, win, 0, &gcvals);

    while (1) {
        XNextEvent(dpy, &event);
        if (event.type == Expose) {
            eye_width = width / 2 - 10;
            eye_height = height / 2 - 10;
            draw_eye(10, 10, eye_width, eye_height, mouse_x, mouse_y);
            draw_eye(width / 2 + 10, 10, eye_width, eye_height, mouse_x, mouse_y);
        } else if (event.type == MotionNotify) {
            mouse_x = event.xmotion.x;
            mouse_y = event.xmotion.y;
            XClearWindow(dpy, win);
            eye_width = width / 2 - 10;
            eye_height = height / 2 - 10;
            draw_eye(10, 10, eye_width, eye_height, mouse_x, mouse_y);
            draw_eye(width / 2 + 10, 10, eye_width, eye_height, mouse_x, mouse_y);
        } else if (event.type == KeyPress) {
            KeySym key = XLookupKeysym(&event.xkey, 0);
            if (key == XK_q || key == XK_Escape) {
                break;
            }
        }
    }

    XFreeGC(dpy, gc);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);

    return 0;
}