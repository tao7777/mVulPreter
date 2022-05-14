 chrm_modification_init(chrm_modification *me, png_modifier *pm,
   const color_encoding *encoding)
 {
    CIE_color white = white_point(encoding);
 
 /* Original end points: */
   me->encoding = encoding;

 /* Chromaticities (in fixed point): */
   me->wx = fix(chromaticity_x(white));
   me->wy = fix(chromaticity_y(white));

   me->rx = fix(chromaticity_x(encoding->red));
   me->ry = fix(chromaticity_y(encoding->red));
   me->gx = fix(chromaticity_x(encoding->green));
   me->gy = fix(chromaticity_y(encoding->green));
   me->bx = fix(chromaticity_x(encoding->blue));
   me->by = fix(chromaticity_y(encoding->blue));

   modification_init(&me->this);
   me->this.chunk = CHUNK_cHRM;
   me->this.modify_fn = chrm_modify;
   me->this.add = CHUNK_PLTE;
   me->this.next = pm->modifications;
   pm->modifications = &me->this;
}
