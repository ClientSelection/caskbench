#include <config.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>

#include "caskbench.h"
#include "cairo-shapes.h"

int
ca_setup_roundrect(caskbench_context_t *ctx)
{
    cairo_t *cr = ctx->cairo_cr;

    cairo_set_antialias (cr, CAIRO_ANTIALIAS_BEST);
    return 1;
}

void
ca_teardown_roundrect(void)
{
}

static void
rounded_rectangle (cairo_t *cr,
                   double x, double y, double w, double h,
                   double radius)
{
    cairo_move_to (cr, x+radius, y);
    cairo_arc (cr, x+w-radius, y+radius,   radius, M_PI + M_PI / 2, M_PI * 2        );
    cairo_arc (cr, x+w-radius, y+h-radius, radius, 0,               M_PI / 2        );
    cairo_arc (cr, x+radius,   y+h-radius, radius, M_PI/2,          M_PI            );
    cairo_arc (cr, x+radius,   y+radius,   radius, M_PI,            270 * M_PI / 180);
}

int
ca_test_roundrect(caskbench_context_t *ctx)
{
    int i;
    double line_width, x, y, radius;
    cairo_t *cr = ctx->cairo_cr;

    for (i=0; i<ctx->size; i++) {
        cairoRandomizeColor(ctx);

        x = 10 + trunc( (((double)ctx->canvas_width-20.0)*rand())/RAND_MAX );
        y = 10 + trunc( (((double)ctx->canvas_height-20.0)*rand())/RAND_MAX );

        /* vary radius upto half of MIN(X,Y) */
        radius = (double)rand()/RAND_MAX * 20;
#if USE_CAIROGLES
        cairo_rounded_rectangle (cr, x, y, 100, 40, radius, radius, radius, radius);
#else
        rounded_rectangle (cr, x, y, 100, 40, radius);
#endif
        /* line_width cannot be more than twice of radius due to skia limitation - Issue #4 in skia https://github.com/Samsung/skia/issues/4 */
        line_width = (double)rand()/RAND_MAX * (2*radius);
        cairo_set_line_width (cr, line_width);

        cairo_stroke (cr);
    }
    return 1;
}
