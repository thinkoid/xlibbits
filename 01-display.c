/* -*- mode: c; -*- */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

int main(void)
{
        int screen;
        Visual *visual;

        Display *dpy = XOpenDisplay(0);
        if (0 == dpy) {
                fprintf(stderr, "XOpenDisplay failed\n");
                exit(1);
        }

        screen = DefaultScreen(dpy);

        printf(" --> default screen : %d\n", screen);
        printf(" --> root window    : %ld\n", RootWindow(dpy, screen));

        printf(" --> dimensions     : %d x %d (depth : %d)\n",
               DisplayWidth(dpy, screen),
               DisplayHeight(dpy, screen),
               DefaultDepth(dpy, screen));

        visual = DefaultVisual(dpy, screen);
        printf(" --> bits per rgb   : %d\n", visual->bits_per_rgb);

        XCloseDisplay(dpy);

        return 0;
}
