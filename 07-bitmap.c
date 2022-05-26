/* -*- mode: c; -*- */

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#define STB_IMAGE_IMPLEMENTATION 1
#include <stb/stb_image.h>

#define UNUSED(x) ((void)(x))

struct img
{
        unsigned char *buf;
        int w, h;
};

static Window
make_window(Display *dpy)
{
        Window win, root;
        int white, black;

        white = WhitePixel(dpy, DefaultScreen(dpy));
        black = BlackPixel(dpy, DefaultScreen(dpy));

        root = RootWindow(dpy, DefaultScreen(dpy));
        win = XCreateSimpleWindow(dpy, root, 0, 0, 640, 480, 1, black, white);

        XMapWindow(dpy, win);
        XFlush(dpy);
        
        return win;
}

static GC
make_gc(Display *dpy, Window win, int reverse)
{
        GC gc;

        unsigned long valuemask = 0;
        XGCValues values;

        int white, black;

        white = WhitePixel(dpy, DefaultScreen(dpy));
        black = BlackPixel(dpy, DefaultScreen(dpy));

        gc = XCreateGC(dpy, win, valuemask, &values);
        if (0 == gc) {
                fprintf(stderr, "XCreateGC failed\n");
                return 0;
        }

        if (reverse) {
                XSetForeground(dpy, gc, white);
                XSetBackground(dpy, gc, black);
        }
        else {
                XSetForeground(dpy, gc, black);
                XSetBackground(dpy, gc, white);
        }

        XSetLineAttributes(dpy, gc, 2, LineSolid, CapButt, JoinBevel);
        XSetFillStyle(dpy, gc, FillSolid);

        return gc;
}

static int
load_image(struct img *img, const char *s)
{
        int ignore;

        img->buf = stbi_load(s, &img->w, &img->h, &ignore, 1);
        if (0 == img->buf) {
                fprintf(stderr, "read image %s failed\n", s);
                return 1;
        }

        return 0;
}

static void
draw_image(Display *dpy, Window win, GC gc, const struct img *img)
{
        int i, j;
        unsigned char *src = img->buf;

        for (i = 0; i < img->h; ++i) {
                for (j = 0; j < img->w; ++j) {
                        if (0 == src[i * img->w + j])
                                XDrawPoint(dpy, win, gc, j + 100, i + 100);
                }
        }
}

static void
event_loop(Display *dpy, Window win, GC gc, const struct img *img)
{
        int running = 1;
        
        XEvent any;
        XKeyEvent *key;

        for (XSync(dpy, 0); running && !XNextEvent(dpy, &any); ) {
                switch (any.type) {
                case KeyPress:
                        key = (XKeyEvent *)&any.xkey;
                        if (XK_q == XKeycodeToKeysym(dpy, key->keycode, 0))
                                running = 0;
                        break;

                case Expose:
                        draw_image(dpy, win, gc, img);
                        break;
                }
        }

        XSync(dpy, 1);
}

int main(int argc, char **argv)
{
        Display *dpy;

        Window win;
        GC gc;

        struct img img;

        if (1 == argc || 0 == argv[1] || 0 == argv[1][0]) {
                printf("usage: program filename\n");
                exit(2);
        }

        if (load_image(&img, argv[1]))
                exit(1);

        dpy = XOpenDisplay(0);
        if (0 == dpy) {
                fprintf(stderr, "XOpenDisplay failed\n");
                goto b;
        }

        win = make_window(dpy);
        XSelectInput(dpy, win, KeyPressMask | ExposureMask | StructureNotifyMask);

        gc = make_gc(dpy, win, 0);
        if (0 == gc)
                goto a;

        draw_image(dpy, win, gc, &img);
        event_loop(dpy, win, gc, &img);
        
        XFreeGC(dpy, gc);
a:
        XUnmapWindow(dpy, win);
        XDestroyWindow(dpy, win);

        XCloseDisplay(dpy);
b:
        free(img.buf);
        
        return 0;
}

