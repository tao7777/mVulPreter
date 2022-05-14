standard_info_part2(standard_display *dp, png_const_structp pp,

     png_const_infop pi, int nImages)
 {
    /* Record cbRow now that it can be found. */
   dp->pixel_size = bit_size(pp, png_get_color_type(pp, pi),
      png_get_bit_depth(pp, pi));
    dp->bit_width = png_get_image_width(pp, pi) * dp->pixel_size;
    dp->cbRow = png_get_rowbytes(pp, pi);
 
 /* Validate the rowbytes here again. */
 if (dp->cbRow != (dp->bit_width+7)/8)
      png_error(pp, "bad png_get_rowbytes calculation");

 /* Then ensure there is enough space for the output image(s). */
   store_ensure_image(dp->ps, pp, nImages, dp->cbRow, dp->h);
}
