#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>

#include "caskbench.h"

static cairo_pattern_t *mask;

int
setup_mask(cairo_t *cr)
{
  mask = cairo_pattern_create_rgba (0, 0, 0, 0.5);
  return 1;
}

void
teardown_mask(void)
{
  cairo_pattern_destroy (mask);
}

int
test_mask(cairo_t *cr)
{
  cairo_mask (cr, mask);
  return 1;
}
