/* -*- mode: c; -*- */

#include <X11/X.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#define UNUSED(x) ((void)(x))

#define SCR(dpy) DefaultScreen(dpy)
#define ROOT(dpy) RootWindow(dpy, SCR(dpy))

#define BLACK(dpy) BlackPixel(dpy, SCR(dpy))
#define WHITE(dpy) WhitePixel(dpy, SCR(dpy))

static Window
make_window(Display *dpy)
{
        Window win;

        win = XCreateSimpleWindow(
                dpy, ROOT(dpy), 0, 0, 640, 480, 1, BLACK(dpy), WHITE(dpy));

        XMapWindow(dpy, win);
        XFlush(dpy);
        
        return win;
}

static const char *event_desc(size_t type)
{
    static const char *arr[] = {
        "0",
        "1",
        "KeyPress", /* 2 */
        "KeyRelease", /* 3 */
        "ButtonPress", /* 4 */
        "ButtonRelease", /* 5 */
        "MotionNotify", /* 6 */
        "EnterNotify", /* 7 */
        "LeaveNotify", /* 8 */
        "FocusIn", /* 9 */
        "FocusOut", /* 10 */
        "KeymapNotify", /* 11 */
        "Expose", /* 12 */
        "GraphicsExpose", /* 13 */
        "NoExpose", /* 14 */
        "VisibilityNotify", /* 15 */
        "CreateNotify", /* 16 */
        "DestroyNotify", /* 17 */
        "UnmapNotify", /* 18 */
        "MapNotify", /* 19 */
        "MapRequest", /* 20 */
        "ReparentNotify", /* 21 */
        "ConfigureNotify", /* 22 */
        "ConfigureRequest", /* 23 */
        "GravityNotify", /* 24 */
        "ResizeRequest", /* 25 */
        "CirculateNotify", /* 26 */
        "CirculateRequest", /* 27 */
        "PropertyNotify", /* 28 */
        "SelectionClear", /* 29 */
        "SelectionRequest", /* 30 */
        "SelectionNotify", /* 31 */
        "ColormapNotify", /* 32 */
        "ClientMessage", /* 33 */
        "MappingNotify", /* 34 */
        "GenericEvent"
    };

    return type < sizeof arr / sizeof *arr
            ? arr[type] : "invalid event type";
}

static void process_events_of(Display *dpy)
{
        XEvent any;
        XKeyEvent *key;

        for (XSync(dpy, 0); !XNextEvent(dpy, &any); ) {
                printf(" --> event : %s\n", event_desc(any.type));

                if (KeyPress == any.type) {
                        /* quit the program when q is pressed */
                        key = (XKeyEvent *)&any.xkey;
                        if (XK_q == XKeycodeToKeysym(dpy, key->keycode, 0))
                                break;
                }
        }

        XSync(dpy, 1);
}

int main(int argc, char **argv)
{
        Display *dpy;
        Window win;

        UNUSED(argc);

        dpy = XOpenDisplay(argv[1]);
        if (0 == dpy) {
                fprintf(stderr, "XOpenDisplay(%s) failed\n",
                        argv[1] ? argv[1] : "null");
                exit(1);
        }

        win = make_window(dpy);

        /* select what events we wish to see */
        XSelectInput(dpy, win, 0
                     | ExposureMask
                     | KeyPressMask
                     | ButtonPressMask
                     | StructureNotifyMask);

        process_events_of(dpy);

        XUnmapWindow(dpy, win);
        XDestroyWindow(dpy, win);
        
        XCloseDisplay(dpy);
        
        return 0;
}
