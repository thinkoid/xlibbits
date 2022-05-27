/* -*- mode: c; -*- */

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#define STB_IMAGE_IMPLEMENTATION 1
#include <stb/stb_image.h>

#define UNUSED(x) ((void)x)

struct drw
{
        Window win;
        GC gc;
};

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

static struct img *
load_image(const char *s)
{
        int ignore;
        struct img *img;

        if (0 == s || 0 == s[0]) {
                fprintf(stderr, "invalid filename\n");
                return 0;
        }

        img = malloc(sizeof *img);
        if (0 == img) {
                fprintf(stderr, "image allocation failed\n");
                return 0;
        }

        img->buf = stbi_load(s, &img->w, &img->h, &ignore, 1);
        if (0 == img->buf) {
                fprintf(stderr, "read image %s failed\n", s);
                free(img);
                return 0;
        }

        return img;
}

static void
draw_image(Display *dpy, struct drw *drw, const struct img *img)
{
        int i, j;
        unsigned char *src = img->buf;

        for (i = 0; i < img->h; ++i) {
                for (j = 0; j < img->w; ++j) {
                        if (0 == src[i * img->w + j])
                                XDrawPoint(dpy, drw->win, drw->gc, j + 100, i + 100);
                }
        }
}

static void
event_loop(Display *dpy, struct drw *drw, const struct img *img)
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
                        draw_image(dpy, drw, img);
                        break;
                }
        }

        XSync(dpy, 1);
}

static Display *
make_display()
{
        Display *dpy;

        dpy = XOpenDisplay(0);
        if (0 == dpy) {
                fprintf(stderr, "XOpenDisplay failed\n");
                return 0;
        }

        return dpy;
}

static struct drw *
make_drawable(Display *dpy)
{
        struct drw *drw;

        drw = malloc(sizeof *drw);
        if (0 == drw) {
                fprintf(stderr, "drawable allocation failed\n");
                goto a;
        }

        drw->win = make_window(dpy);
        XSelectInput(dpy, drw->win, 0
                     | KeyPressMask
                     | ExposureMask
                     | StructureNotifyMask);

        drw->gc = make_gc(dpy, drw->win, 0);
        if (0 == drw->gc)
                goto b;

        return drw;

b:
        XUnmapWindow(dpy, drw->win);
        XDestroyWindow(dpy, drw->win);

        free(drw);
a:
        return 0;
}

int main(int argc, char **argv)
{
        int ret = 0;

        Display *dpy;

        struct drw *drw;
        struct img *img;

        UNUSED(argc);

        img = load_image(argv[1]);
        if (0 == img)
                exit(1);

        dpy = make_display();
        if (0 == dpy) {
                ret = 1;
                goto a;
        }

        drw = make_drawable(dpy);
        if (0 == drw) {
                ret = 1;
                goto b;
        }

        draw_image(dpy, drw, img);
        event_loop(dpy, drw, img);

        XFreeGC(dpy, drw->gc);

        XUnmapWindow(dpy, drw->win);
        XDestroyWindow(dpy, drw->win);

        free(drw);
b:
        XCloseDisplay(dpy);
a:
        free(img->buf);
        free(img);

        return ret;
}
