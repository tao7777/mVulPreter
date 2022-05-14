modifier_current_encoding(PNG_CONST png_modifier *pm, color_encoding *ce)
 {
    if (pm->current_encoding != 0)
       *ce = *pm->current_encoding;

 else
      memset(ce, 0, sizeof *ce);

   ce->gamma = pm->current_gamma;

 }
