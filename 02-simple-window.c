/* -*- mode: c; -*- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>

int main()
{
        int screen;

        Display *dpy;
        Window win;

        dpy = XOpenDisplay(0);
        if (0 == dpy) {
                fprintf(stderr, "XOpenDisplay failed\n");
                exit(1);
        }

        screen = DefaultScreen(dpy);

        win = XCreateSimpleWindow(
                dpy, RootWindow(dpy, screen),
                /*
                 * position, width, height, border width, will likely be
                 * changed by the window manager
                 */
                0, 0, 640, 480, 1,
                BlackPixel(dpy, screen), WhitePixel(dpy, screen));

        XMapWindow(dpy, win);
        XFlush(dpy);

        sleep(3);

        XCloseDisplay(dpy);

        return 0;
}
