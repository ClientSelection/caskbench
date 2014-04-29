// From http://www.atoker.com/blog/2008/09/06/skia-graphics-library-in-chrome-first-impressions/
#include <config.h>

#include <SkCanvas.h>
#include <SkPaint.h>
#include <SkOSFile.h>
#include <SkImageDecoder.h>

#include "kinetics.h"
#include "caskbench.h"
#include "skia-shapes.h"

static kinetics_t *particles;
static int element_spacing;
static int num_x_elements;
static int num_y_elements;
SkPath path;

static void drawClip(caskbench_context_t *ctx,double x,double y,double clipr=0)
{
    int old_x, old_y,old_width, old_height;
    old_width = ctx->shape_args.width;
    old_height = ctx->shape_args.height;

    int i,shape,p;
    double r;
	path.reset();
    r = 0.9 * element_spacing /2;
    if(!ctx->shape_args.shape_id)
        shape = ((4.0 * rand())/RAND_MAX) +1;
    else
        shape = ctx->shape_args.shape_id ;
    switch (shape) {
    case 1:
        // Circle
        ctx->shape_args.center_x = x+r;
        ctx->shape_args.center_y = y+r;
        ctx->shape_args.radius = r;
        path.addCircle(ctx->shape_args.center_x, ctx->shape_args.center_y,SkIntToScalar(clipr));
        break;

    case 2:
        // Rectangle
        ctx->shape_args.center_x =  x;
        ctx->shape_args.center_y = y;
        ctx->shape_args.width = (ctx->shape_args.center_x) +((ctx->shape_args.width)?ctx->shape_args.width:2*clipr);
        ctx->shape_args.height = (ctx->shape_args.center_y) + ((ctx->shape_args.height)?ctx->shape_args.height:2*clipr);
        path.addRect(ctx->shape_args.center_x, ctx->shape_args.center_y, ctx->shape_args.width, ctx->shape_args.height);
        ctx->shape_args.width = old_width;
        ctx->shape_args.height = old_height;
        break;

    case 3:
        // Triangle
        ctx->shape_args.numpoints = 3;
        ctx->shape_args.points = (double (*)[2]) malloc(ctx->shape_args.numpoints*2*(sizeof(double)));
        ctx->shape_args.points[0][0] = x;
        ctx->shape_args.points[0][1] = y+2*clipr;
        ctx->shape_args.points[1][0] = 2*clipr;
        ctx->shape_args.points[1][1] = 0;
        ctx->shape_args.points[2][0] = -clipr;
        ctx->shape_args.points[2][1] = -2*clipr;
        for ( p = 0; p <  ctx->shape_args.numpoints; p++ ) {
            if(p == 0)
                path.moveTo(ctx->shape_args.points[p][0], ctx->shape_args.points[p][1]);
            else
                path.rLineTo(ctx->shape_args.points[p][0], ctx->shape_args.points[p][1]);
        }

        free (ctx->shape_args.points);

        break;
    case 4:
        // Star
        ctx->shape_args.numpoints = 10;
        ctx->shape_args.points = (double (*)[2]) malloc(ctx->shape_args.numpoints*2*(sizeof(double)));

        for (p = 0; p < 10; p++ ) {
            int px = x + 2*r * star_points[p][0]/200.0;
            int py = y + 2*r * star_points[p][1]/200.0;
            ctx->shape_args.points[p][0] = px;
            ctx->shape_args.points[p][1] = py;
        }
        for (p = 0; p <  ctx->shape_args.numpoints; p++ ) {
            if(p == 0)
                path.moveTo(ctx->shape_args.points[p][0], ctx->shape_args.points[p][1]);
            else
                path.lineTo(ctx->shape_args.points[p][0], ctx->shape_args.points[p][1]);
        }
        free (ctx->shape_args.points);
        break;

    default:
        break;
    }
}

static void drawShape(caskbench_context_t *ctx,double x,double y,double clipr=0)
{
    int old_x, old_y,old_width, old_height;
    old_width = ctx->shape_args.width;
    old_height = ctx->shape_args.height;

    int i,shape,p;
    double r;
    r = 0.9 * element_spacing /2;
    if(!ctx->shape_args.shape_id)
        shape = ((4.0 * rand())/RAND_MAX) +1;
    else
        shape = ctx->shape_args.shape_id ;
    ctx->shape_args.center_x = x;
    ctx->shape_args.center_y = y;
    ctx->shape_args.radius = r;
    ctx->shape_args.width = (ctx->shape_args.width)?ctx->shape_args.width:2*r;
    ctx->shape_args.height = (ctx->shape_args.height)?ctx->shape_args.height:2*r;
    skiaShapes[(shape-1)%4](ctx,&ctx->shape_args);
    ctx->skia_canvas->flush();

}

static bool
draw_square (caskbench_context_t *ctx,SkCanvas* canvas, int x, int y) {

    SkShader* shader;
    SkBitmap    bm;
    int clipr = 30;
    //    canvas->save();
    //    canvas->translate(SkIntToScalar(x), SkIntToScalar(y));

    if (ctx->shape_args.image_path == NULL)
        return false;
    SkImageDecoder::DecodeFile(ctx->shape_args.image_path, &bm);
    shader = SkShader::CreateBitmapShader(bm, SkShader::kClamp_TileMode,
            SkShader::kClamp_TileMode);

    ctx->skia_paint->setShader (shader);
    drawClip(ctx,x,y,clipr);
    canvas->clipPath(path, SkRegion::kReplace_Op, true);
    drawShape(ctx,x,y);
    ctx->skia_canvas->flush();
    if(shader != NULL)
    {
        ctx->skia_paint->setShader (NULL);
        delete shader;
    }
    path.reset();
    return true;

}

static bool
draw_clip_tests (caskbench_context_t *ctx,SkCanvas* canvas,kinetics_t* particles) {
    int i,j,x,y;
    double red,green,blue,alpha;

    for (j=0; j<num_y_elements; j++) {
        y = particles?particles->y : j * element_spacing;
        for (i=0; i<num_x_elements; i++) {
            x = particles?particles->x : i * element_spacing;
            skiaRandomizeColor(ctx);
            if (!draw_square(ctx,canvas, x,y))
                return false;
        }
    }
    return true;
}

int
sk_setup_clip(caskbench_context_t *ctx)
{
    element_spacing = sqrt( ((double)ctx->canvas_width * ctx->canvas_height) / ctx->size);
    num_x_elements = ctx->canvas_width / element_spacing;
    num_y_elements = ctx->canvas_height / element_spacing;

    return 1;
}

void
sk_teardown_clip(void)
{
}

int
sk_test_clip(caskbench_context_t *ctx)
{
    /* Animation */
    if(ctx->shape_args.animation)
    {
        int num_particles = ctx->shape_args.animation;
        double start_frame, stop_frame, delta;
        particles = (kinetics_t *) malloc (sizeof (kinetics_t) * num_particles);
        int i,j ;
        for (i = 0; i < num_particles; i++)
            kinetics_init (&particles[i]);
        delta = 0;

        for (j=0;j<num_particles;j++){
            ctx->skia_canvas->drawColor(SK_ColorBLACK);
            for (i = 0; i < num_particles; i++) {
                kinetics_update(&particles[i], 0.1);
                if (!draw_clip_tests(ctx,ctx->skia_canvas,&particles[i]))
                    return 0;
            }
        }
    }
    /* Static clip */
    else {
        if (!draw_clip_tests(ctx,ctx->skia_canvas,NULL))
            return 0;
    }

    return 1;
}
