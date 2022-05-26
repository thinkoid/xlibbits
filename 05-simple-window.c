/* -*- mode: c; -*- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>

#define UNUSED(x) ((void)(x))

int main(int argc, char **argv)
{
        int scr;
        
        Display *dpy;
        Window win;
        
        UNUSED(argc);

        dpy = XOpenDisplay(argv[1]);
        if (0 == dpy) {
                fprintf(stderr, "XOpenDisplay(%s) failed\n",
                        argv[1] ? argv[1] : "null");
                exit(1);
        }

        scr = DefaultScreen(dpy);

        win = XCreateSimpleWindow(
                dpy, RootWindow(dpy, scr),
                /*
                 * position, width, height, border width, will likely be
                 * changed by the window manager
                 */
                0, 0, 640, 480, 1,
                BlackPixel(dpy, scr), WhitePixel(dpy, scr));

        XMapWindow(dpy, win);
        XFlush(dpy);

        sleep(3);

        XCloseDisplay(dpy);
        
        return 0;
}
