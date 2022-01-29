#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <X11/Xlib.h>

#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>

#include <cairo.h>
#include <cairo-xlib.h>

char* program_name;

Display* d;
Window overlay;
Window root;
cairo_t* cr;
int width, height;
int shape = 0;
int x = 10;
int y = 10;
int w = 20;
int h = 20;
int r = 10;
double c[3] = {1.0, 0.0, 0.0};
int running = 1;

void usage()
{
    printf ("Usage: %s [SHAPE] [OPTION...]\n\n", program_name);
    switch (shape) {
        case 1:
            printf ("\
OPTIONS:\n\
-x, --posx\tset x position of the top-left corner of a rectangle\n\
-y, --posy\tset y position of the top-left corner of a rectangle\n\
-w, --width\tset rectangle width\n\
-h, --height\tset rectangle height\n\
-c, --color\tset color (RGB hex value) of a rectangle (e.g ff0000 for red)\n");
            break;
        case 2:
            printf ("\
OPTIONS:\n\
-x, --posx\tset x position of the center of a cricle\n\
-y, --posy\tset y position of the center of a circle\n\
-r, --radius\tset circle radius\n\
-c, --color\tset color (RGB hex value) of a circle (e.g ff0000 for red)\n");
            break;
        default:
            printf ("\
SHAPES:\n\
rectangle\n\
circle\n");
    }
    printf ("\nuse '%s [SHAPE] --help' for available options\n", program_name);
    exit(0);
}

void draw()
{
    cairo_set_source_rgb(cr, c[0], c[1], c[2]);
    switch (shape) {
        case 1:
            cairo_rectangle(cr, x, y, w, h);
            break;
        case 2:
            cairo_arc(cr, x, y, r, 0, 2 * M_PI);
            break;
        default:
            exit(1);
    }
    cairo_fill(cr);
}

void interrupt(int sig)
{
    printf("\nexiting\n");
    running = 0;
}

int main(int argc, char** argv)
{
    program_name = argv[0];

    if (argc < 2)
        usage();

    if (strcmp(argv[1], "rectangle") == 0)
        shape = 1;
    else if (strcmp(argv[1], "circle") == 0)
        shape = 2;
    else
        usage();

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0)
            usage();
        if (strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--posx") == 0)
            x = atoi(argv[++i]);
        else if (strcmp(argv[i], "-y") == 0 || strcmp(argv[i], "--posy") == 0)
            y = atoi(argv[++i]);
        else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--width") == 0)
            w = atoi(argv[++i]);
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--height") == 0)
            h = atoi(argv[++i]);
        else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--radius") == 0)
            r = atoi(argv[++i]);
        else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--color") == 0) {
            char colors[3][3];
            ++i;
            for (int j = 0; j < 3; ++j) {
                for(int k = 0; k < 2; ++k)
                    colors[j][k] = argv[i][j * 2 + k];
                colors[j][2] = '\0';
                c[j] = (double)strtol(colors[j], NULL, 16)/255;
            }
        }
    }

    d = XOpenDisplay(NULL);

    int s = DefaultScreen(d);
    root = RootWindow(d, s);

    XCompositeRedirectSubwindows(d, root, CompositeRedirectAutomatic);
    XSelectInput(d, root, SubstructureNotifyMask);

    width = DisplayWidth(d, s);
    height = DisplayHeight(d, s);

    overlay = XCompositeGetOverlayWindow(d, root);

    // Allow input passthrough for overlay.
    XserverRegion region = XFixesCreateRegion(d, NULL, 0);
    XFixesSetWindowShapeRegion(d, overlay, ShapeBounding, 0, 0, 0);
    XFixesSetWindowShapeRegion(d, overlay, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(d, region);

    cairo_surface_t* surf = cairo_xlib_surface_create(d, overlay,
            DefaultVisual(d, s),
            width, height);
    cr = cairo_create(surf);

    XSelectInput(d, overlay, ExposureMask);

    signal(SIGINT, interrupt);

    switch (shape) {
        case 1:
            printf("drawing %dx%d rectangle at %dx%d\n", w, h, x, y);
            break;
        case 2:
            printf("drawing circle of radius %d at %dx%d\n", r, x, y);
    }

    draw();

    while (running) {
        overlay = XCompositeGetOverlayWindow(d, root);
        draw();
        XCompositeReleaseOverlayWindow(d, root);
    }

    cairo_destroy(cr);
    cairo_surface_destroy(surf);
    XCloseDisplay(d);

    return 0;
}
