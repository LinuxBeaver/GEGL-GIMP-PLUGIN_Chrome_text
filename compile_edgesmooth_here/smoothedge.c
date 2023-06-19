/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * 2022 Beaver Edge Smooth
 */

#include "config.h"
#include <glib/gi18n-lib.h>


#ifdef GEGL_PROPERTIES

#define TUTORIAL \
"  id=1  gegl:over aux=[ ref=1  xor aux=[  ref=1  median-blur radius=2.4 alpha-percentile=2    ]  "\

property_string (string, _("custom gegl graph"), TUTORIAL)
      ui_meta     ("role", "output-extent")




property_int  (radius, _("Radius"), 2)
  value_range (-400, 400)
  ui_range    (0, 100)
  ui_meta     ("unit", "pixel-distance")
  description (_("Neighborhood radius, a negative value will calculate with inverted percentiles"))
    ui_meta     ("role", "output-extent")

property_double  (alpha_percentile, _("Alpha percentile"), 2)
  value_range (0, 100)
  description (_("Neighborhood alpha percentile"))
      ui_meta     ("role", "output-extent")



property_int  (radius2, _("Radius"), 2)
  value_range (-20, 20)
  ui_range    (0, 100)
  ui_meta     ("unit", "pixel-distance")
  description (_("Neighborhood radius, a negative value will calculate with inverted percentiles"))
  ui_meta     ("role", "output-extent")

property_double  (percentile2, _("Percentile"), 1)
  value_range (0, 100)
  description (_("Neighborhood color percentile"))
   ui_meta     ("role", "output-extent")

property_double  (alpha_percentile2, _("Median edges"), 73)
  value_range (0, 100)
  description (_("Neighborhood alpha percentile"))


property_int  (radius3, _("Radius"), 2)
  value_range (-400, 400)
  ui_range    (0, 100)
  ui_meta     ("unit", "pixel-distance")
  description (_("Neighborhood radius, a negative value will calculate with inverted percentiles"))
  ui_meta     ("role", "output-extent")


property_double (gaus, _("Blur Edge"), 1)
   description (_("Standard deviation for the horizontal axis"))
   value_range (0.0, 3.0)
   ui_range    (0.24, 3.0)
   ui_gamma    (3.0)
   ui_meta     ("unit", "pixel-distance")
   ui_meta     ("axis", "x")


property_double (value, _("Increase Opacity"), 1.2)
    description (_("Global opacity value that is always used on top of the optional auxiliary input buffer."))
    value_range (1, 6.0)
    ui_range    (1, 3.5)



#else

#define GEGL_OP_META
#define GEGL_OP_NAME     smoothedge
#define GEGL_OP_C_SOURCE smoothedge.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *over, *xor, *graph, *behind, *median, *median2, *median3, *gaussian, *opacity;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");

  over    = gegl_node_new_child (gegl,
                                  "operation", "gegl:over",
                                  NULL);

  xor    = gegl_node_new_child (gegl,
                                  "operation", "gegl:xor",
                                  NULL);

  median    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur",
                                  NULL);

  median2    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur",
                                  NULL);

  median3    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur",
                                  NULL);


  gaussian    = gegl_node_new_child (gegl,
                                  "operation", "gegl:gaussian-blur",
                                  NULL);

  opacity    = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity",
                                  NULL);

  behind    = gegl_node_new_child (gegl,
                                  "operation", "gegl:dst-atop",
                                  NULL);

  graph    = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl",
                                  NULL);






gegl_node_link_many(input, graph, behind, output, NULL);
gegl_node_link_many(input,  median, gaussian, opacity, median2,  NULL);
gegl_node_connect_from (behind, "aux", median2, "output"); 

  gegl_operation_meta_redirect (operation, "gaus", gaussian, "std-dev-x");

  gegl_operation_meta_redirect (operation, "gaus", gaussian, "std-dev-y");

  gegl_operation_meta_redirect (operation, "radius", median, "radius");

  gegl_operation_meta_redirect (operation, "radius2", median2, "radius");

  gegl_operation_meta_redirect (operation, "radius3", median3, "radius");

  gegl_operation_meta_redirect (operation, "percentile2", median2, "percentile");

  gegl_operation_meta_redirect (operation, "alpha_percentile", median, "alpha-percentile");

  gegl_operation_meta_redirect (operation, "alpha_percentile2", median2, "alpha-percentile");

  gegl_operation_meta_redirect (operation, "value", opacity, "value");

  gegl_operation_meta_redirect (operation, "string", graph, "string");





}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:edgesmooth",
    "title",       _("Rough Edge Smoother"),
    "categories",  "EdgeSmoother",
    "reference-hash", "45ed5656a11bgxxdt27730vaefe2g4f1b2ac",
    "description", _("GEGL will apply a median blur and a few other things around a transparent images edges "
                     ""),
    NULL);
}

#endif
