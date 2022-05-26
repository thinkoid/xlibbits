/* -*- mode: c; -*- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>

#define UNUSED(x) ((void)(x))

int main(int argc, char **argv)
{
        Display *dpy;

        UNUSED(argc);

        dpy = XOpenDisplay(argv[1]);
        if (0 == dpy) {
                fprintf(stderr, "XOpenDisplay(%s) failed\n",
                        argv[1] ? argv[1] : "null");
                exit(1);
        }

        printf(" --> default screen : %d\n", DefaultScreen(dpy));
        printf(" --> root window    : %ld\n", RootWindow(dpy, DefaultScreen(dpy)));

        XCloseDisplay(dpy);
        
        return 0;
}
