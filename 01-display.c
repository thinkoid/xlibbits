/* -*- mode: c; -*- */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#define UNUSED(x) ((void)(x))

static Display *
open_display(const char *s)
{
        Display *dpy = XOpenDisplay(s);

        if (0 == dpy) {
                fprintf(stderr, "display open failed (%s)\n", s && s[0] ? s : "null");
        }

        return dpy;
}

int main(int argc, char **argv)
{
        Display *dpy;

        UNUSED(argc);

        if (0 == (dpy = open_display(argv[1])) ||
            0 == (dpy = open_display(getenv("DISPLAY")))) {
                exit(1);
        }

        XCloseDisplay(dpy);

        return 0;
}
