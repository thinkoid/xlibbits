/* -*- mode: c; -*- */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <X11/Xft/Xft.h>

#define ASSERT assert
#define UNUSED(x) ((void)x)

#define SCR(dpy) (DefaultScreen(dpy))
#define ROOT(dpy) (RootWindow(dpy, SCR(dpy)))

#define DEPTH(dpy) DefaultDepth(DPY, SCR(dpy))

#define VISUAL(dpy) (DefaultVisual(dpy, SCR(dpy)))
#define COLORMAP(dpy) (DefaultColormap(dpy, SCR(dpy)))

#define WHITE(dpy) WhitePixel(dpy, SCR(dpy))
#define BLACK(dpy) BlackPixel(dpy, SCR(dpy))

static Display *
make_display(const char *s)
{
        Display *dpy;

        if (0 == (dpy = XOpenDisplay(s))) {
                fprintf(stderr, "XOpenDisplay(%s) failed\n", s ? s : ":0");
                exit(1);
        }

        return dpy;
}

static GC
make_gc(Display *dpy, Window win)
{
        GC gc;

        gc = XCreateGC(dpy, win, 0, 0);
        XSetLineAttributes(dpy, gc, 1, LineSolid, CapButt, JoinMiter);

        return gc;
}

static Window
make_window(Display *dpy)
{
        Window win;

        win = XCreateSimpleWindow(
                dpy, ROOT(dpy), 0, 0, 640, 480, 1, BLACK(dpy), WHITE(dpy));

        XMapWindow(dpy, win);
        XFlush(dpy);

        XSelectInput(dpy, win, 0
                     | KeyPressMask
                     | ExposureMask
                     | StructureNotifyMask);

        return win;
}

static XftFont *
make_xft_font(Display *dpy, const char *s)
{
        XftFont *font;

        if (0 == s || 0 == s[0])
                s = "Monospace-12";

        font = XftFontOpenName(dpy, SCR(dpy), s);
        if (0 == font) {
                fprintf(stderr, "cannot load font %s\n", s);
                return 0;
        }

        return font;
}

static int
make_xft_colors(Display *dpy, XftColor *colors, size_t n)
{
        size_t i;

        static const char *xs[] = { "#222222", "#93a660" };
        ASSERT(n == sizeof xs / sizeof *xs);

        for (i = 0; i < n; ++i) {
                if (!XftColorAllocName(dpy, VISUAL(dpy), COLORMAP(dpy), xs[i], colors + i)) {
                        fprintf(stderr, "color %s allocation failed", xs[i]);
                        goto end;
                }
        }

        return 0;

end:
        while (i--)
                XftColorFree(dpy, VISUAL(dpy), COLORMAP(dpy), colors + i);

        return 1;
}

static XftDraw *
make_xft_drawable(Display *dpy, Window win)
{
        XftDraw *drw;

        drw = XftDrawCreate(dpy, win, VISUAL(dpy), COLORMAP(dpy));
        if (0 == drw) {
                fprintf(stderr, "XftDrawCreate failed");
                return 0;
        }

        return drw;
}

static int
geometry_of(Display *dpy, Drawable drw, unsigned *w, unsigned *h)
{
        int x, y;
        unsigned bw, depth;

        Window ignore;

        if (!XGetGeometry(dpy, drw, &ignore, &x, &y, w, h, &bw, &depth)) {
                fprintf(stderr, "drawable geometry unavailable");
                return 1;
        }

        return 0;
}

static void
draw_text(const char *s,
          Display *dpy, Window win, GC gc,
          XftFont *font, XftDraw *draw, XftColor *fg, XftColor *bg)
{
        int x, y, a, d;
        unsigned w, h;

        XGlyphInfo info;

        if (geometry_of(dpy, win, &w, &h))
                return;

        XSetForeground(dpy, gc, bg->pixel);
        XFillRectangle(dpy, win, gc, 0, 0, w, h);

        XftTextExtentsUtf8(dpy, font, (XftChar8*)s, strlen(s), &info);

        a = font->ascent;
        d = font->descent;

        x = (int)w < info.xOff ? 0 : w / 2 - info.xOff / 2;
        y = (int)h < (a + d) ? (int)(a + d) : ((int)h / 2 + (a + d) / 2);

        XftDrawStringUtf8(draw, fg, font, x, y, (XftChar8*)s, strlen(s));
        XSync(dpy, 0);
}

int main(int argc, char **argv)
{
        Display *dpy;

        Window win;
        GC gc;

        XftFont *font;
        XftColor colors[2];

        XftDraw *draw;

        int running = 1;

        XEvent any;
        XKeyEvent *key;

        UNUSED(argc);
        UNUSED(argv);

        if (0 == (dpy = make_display(0)))
                exit(1);

        win = make_window(dpy);
        gc  = make_gc(dpy, win);

        font = make_xft_font(dpy, argv[1]);
        if (0 == font)
                goto a;

        if (make_xft_colors(dpy, colors, 2))
                goto b;

        draw = make_xft_drawable(dpy, win);
        if (0 == draw)
                goto c;

        draw_text("All your base are belong to us",
                  dpy, win, gc, font, draw, colors + 1, colors);

        for (XSync(dpy, 0); running && !XNextEvent(dpy, &any); ) {
                switch (any.type) {
                case KeyPress:
                        key = (XKeyEvent *)&any.xkey;
                        if (XK_q == XKeycodeToKeysym(dpy, key->keycode, 0))
                                running = 0;
                        break;

                case Expose:
                        draw_text("All your base are belong to us",
                                  dpy, win, gc, font, draw, colors + 1, colors);
                        break;
                }
        }

c:
        XftColorFree(dpy, VISUAL(dpy), COLORMAP(dpy), colors);
        XftColorFree(dpy, VISUAL(dpy), COLORMAP(dpy), colors + 1);
b:
        XftFontClose(dpy, font);
a:
        XFreeGC(dpy, gc);

        XUnmapWindow(dpy, win);
        XDestroyWindow(dpy, win);

        XCloseDisplay(dpy);

        return 0;
}
