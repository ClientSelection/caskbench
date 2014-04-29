#ifndef __CAIRO_SHAPES_H_
#define __CAIRO_SHAPES_H_

#include "forward.h"
#include "shapes.h"
#include "caskbench.h"

void cairoRandomizeColor(caskbench_context_t *cr);

void cairoDrawCircle(caskbench_context_t *ctx, shapes_t *args);

void cairoDrawRectangle(caskbench_context_t *ctx, shapes_t *args);

void cairoDrawTriangle(caskbench_context_t *ctx, shapes_t *args);

void cairoDrawStar(caskbench_context_t *ctx, shapes_t *args);

void cairoDrawRoundedRectangle (caskbench_context_t *ctx, shapes_t *args);

extern void (*cairoShapes[5])(caskbench_context_t *ctx , shapes_t *args);

#endif
