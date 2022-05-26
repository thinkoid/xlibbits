/* -*- mode: c; -*- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>

#define UNUSED(x) ((void)(x))

int main(int argc, char **argv)
{
        Display *dpy;
        
        Screen *screen;
        Visual *visual;

        UNUSED(argc);

        dpy = XOpenDisplay(argv[1]);
        if (0 == dpy) {
                fprintf(stderr, "XOpenDisplay(%s) failed\n",
                        argv[1] ? argv[1] : "null");
                exit(1);
        }

        screen = ScreenOfDisplay(dpy, DefaultScreen(dpy));
        
        printf(" --> default screen : %d\n", DefaultScreen(dpy));
        printf("   --> dimensions   : %d x %d (depth : %d)\n",
               WidthOfScreen(screen), HeightOfScreen(screen),
               DefaultDepthOfScreen(screen));

        visual = DefaultVisual(dpy, DefaultScreen(dpy));
        printf("   --> bits per rgb : %d\n", visual->bits_per_rgb);

        XCloseDisplay(dpy);
        
        return 0;
}
