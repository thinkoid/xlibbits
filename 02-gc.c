/* -*- mode: c; -*- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>

#define UNUSED(x) ((void)(x))

#define DPY (display())
#define GC_ (gc())

#define SCRN (DefaultScreen(DPY))
#define ROOT (RootWindow(DPY, SCRN))

struct display
{
        Display *dpy;
        GC gc;
};

static struct display *dpy /* = 0 */;

static Display *display(void)
{
        return dpy->dpy;
}

static GC gc(void)
{
        return dpy->gc;
}

static void make_display(const char *s)
{
        if (0 == dpy) {
                dpy = malloc(sizeof *dpy);

                if (0 == (dpy->dpy = XOpenDisplay(s))) {
                        fprintf(stderr, "XOpenDisplay(%s) failed\n",
                                s ? s : ":0");
                        exit(1);
                }

                dpy->gc = XCreateGC(DPY, ROOT, 0, 0);
                XSetLineAttributes(DPY, dpy->gc, 1, LineSolid, CapButt, JoinMiter);
        }
}

static void free_display(void)
{
        if (dpy) {
                if (dpy->gc)
                        XFreeGC(DPY, dpy->gc);

                if (dpy->dpy)
                        XCloseDisplay(dpy->dpy);
                
                free(dpy);
                dpy = 0;
        }
}

int main(void)
{
        make_display(0);
        atexit(free_display);

        printf(" --> display : 0x%p\n", (void*)DPY);
        printf(" --> GC      : 0x%p\n", (void*)GC_);

        return 0;
}
