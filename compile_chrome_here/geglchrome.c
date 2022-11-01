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
 * 2022 GEGL chrome metal text styler - Beaver
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_color (value, _("Color"), "#ffb386")
    description (_("The color to paint over the input"))
      ui_meta     ("role", "output-extent")

property_double (bvradius, _("Radius of bevel (higher for larger text)"), 7.0)
  value_range (5.0, 15.0)
  ui_range (5.0, 9)
  ui_gamma (1.5)



property_double (bevelconfig, _("Depth Angle (larger text needs lower)"), 58.0)
    description (_("Elevation angle (degrees)"))
    value_range (45, 80)
    ui_meta ("unit", "degree")


property_double (azimuth, _("Rotate Lighting to alter chrome effect"), 60.0)
    description (_("Light angle (degrees)"))
    value_range (28, 350)
    ui_meta ("unit", "degree")
    ui_meta ("direction", "ccw")

property_double (solar1, _("Solarization of Red Channel"), 2.7)
  value_range (2, 2.8)
  ui_meta     ("sensitive", "! cpn-1-keep")
  ui_meta     ("label", "[color-model {rgb} : rgb-label,"
                        " color-model {hsl} : hsl-label]")
  ui_meta     ("rgb-label", _("Red frequency"))
  ui_meta     ("hsl-label", _("Hue frequency"))

property_double  (solar2, _("Solarization of Green Channel"), 2.8)
  value_range (2.2, 2.8)
  ui_meta     ("sensitive", "! cpn-2-keep")
  ui_meta     ("label", "[color-model {rgb} : rgb-label,"
                        " color-model {hsl} : hsl-label]")
  ui_meta     ("rgb-label", _("Green frequency"))
  ui_meta     ("hsl-label", _("Saturation frequency"))

property_double  (solar3, _("Solarization of Blue Channel"), 2.1)
  value_range (0.0, 4.0)
  ui_meta     ("sensitive", "! cpn-3-keep")
  ui_meta     ("label", "[color-model {rgb} : rgb-label,"
                        " color-model {hsl} : hsl-label]")
  ui_meta     ("rgb-label", _("Blue frequency"))
  ui_meta     ("hsl-label", _("Lightness frequency"))

property_double (lightmetal, _("Darkness to Light"), 0.0)
   description  (_("Lightness adjustment"))
   value_range  (-10.0, 7.0)

property_int  (smoothmetal, _("Smooth metal"), 2)
  description (_("Smoothness"))
  value_range (0, 7)
  ui_range    (0, 7)

property_double (glow, _("Glow effect (bloom)"), 6.0)
    description (_("Glow strength"))
    value_range (0.0, G_MAXDOUBLE)
    ui_range    (0.0, 14.0)

property_double (softglow, _("Soft glow effect"), 3.0)
    value_range (1.0, 7.0)
    ui_meta    ("unit", "pixel-distance")

property_double (sharpen, _("Sharpen Metal"), 0.2)
    description(_("Scaling factor for unsharp-mask, the strength of effect"))
    value_range (0.0, 0.9)
    ui_range    (0.0, 0.9)
    ui_gamma    (3.0)

property_double  (smoothedge, _("Median to smooth rough edges"), 50)
  value_range (0, 100)
  description (_("Neighborhood alpha percentile"))

property_double (opacity, _("Above 100% opacity for edges"), 1.0)
    description (_("Global opacity value that is always used on top of the optional auxiliary input buffer."))
    value_range (1, 3)
    ui_range    (1, 3)

property_double (th, _("Low looks nicer but makes unspaced text fuse"), 0.210)
  value_range (0.195, 0.230)
  ui_range (0.195, 0.230)



#else

#define GEGL_OP_META
#define GEGL_OP_NAME     geglchrome
#define GEGL_OP_C_SOURCE geglchrome.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *color, *bevel, *metallic, *glow, *sharpen, *softglow, *noisereduction, *smoothedge, *opacity;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");


  color    = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                  NULL);

  bevel    = gegl_node_new_child (gegl,
                                  "operation", "gegl:bevel",
                                  NULL);

  metallic    = gegl_node_new_child (gegl,
                                  "operation", "gegl:metallic",
                                  NULL);

  glow    = gegl_node_new_child (gegl,
                                  "operation", "gegl:bloom",
                                  NULL);

  sharpen    = gegl_node_new_child (gegl,
                                  "operation", "gegl:unsharp-mask",
                                  NULL);

  softglow    = gegl_node_new_child (gegl,
                                  "operation", "gegl:softglow",
                                  NULL);

  noisereduction    = gegl_node_new_child (gegl,
                                  "operation", "gegl:noise-reduction",
                                  NULL);

  smoothedge    = gegl_node_new_child (gegl,
                                  "operation", "gegl:edgesmooth",
                                  NULL);

  opacity    = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity",
                                  NULL);






  gegl_node_link_many (input, color, bevel, metallic, glow, sharpen, softglow, noisereduction, smoothedge, opacity, output, NULL);



  gegl_operation_meta_redirect (operation, "value", color, "value");
  gegl_operation_meta_redirect (operation, "th", bevel, "th");
  gegl_operation_meta_redirect (operation, "azimuth", bevel, "azimuth");
  gegl_operation_meta_redirect (operation, "bvradius", bevel, "radius1");
  gegl_operation_meta_redirect (operation, "bevelconfig", bevel, "bevel1");
  gegl_operation_meta_redirect (operation, "solar1", metallic, "solar1");
  gegl_operation_meta_redirect (operation, "solar2", metallic, "solar2");
  gegl_operation_meta_redirect (operation, "solar3", metallic, "solar3");
  gegl_operation_meta_redirect (operation, "lightmetal", metallic, "light");
  gegl_operation_meta_redirect (operation, "smoothmetal", metallic, "smooth");
  gegl_operation_meta_redirect (operation, "glow", glow, "strength");
  gegl_operation_meta_redirect (operation, "sharpen", sharpen, "scale");
  gegl_operation_meta_redirect (operation, "softglow", softglow, "glow-radius");
  gegl_operation_meta_redirect (operation, "smoothedge", smoothedge, "alpha-percentile2");
  gegl_operation_meta_redirect (operation, "opacity", opacity, "value");

  
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:chrome",
    "title",       _("Chrome metal text styler"),
    "categories",  "artistic",
    "reference-hash", "450056eat8b351dt25700ftevfe2g4fonf1c",
    "description", _(""
                     "Make metal chrome text styling effects with GEGL. Works best on 100-500p text. Fails on very small text and wide and excessively bold fonts."),
    NULL);
}

#endif
