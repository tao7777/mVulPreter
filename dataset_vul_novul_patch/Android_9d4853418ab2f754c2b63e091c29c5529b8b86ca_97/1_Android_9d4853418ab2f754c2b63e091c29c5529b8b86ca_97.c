make_size(png_store* PNG_CONST ps, png_byte PNG_CONST colour_type, int bdlo,
    int PNG_CONST bdhi)
 {
    for (; bdlo <= bdhi; ++bdlo)
    {
      png_uint_32 width;

 for (width = 1; width <= 16; ++width)
 {
         png_uint_32 height;

 for (height = 1; height <= 16; ++height)
 {
 /* The four combinations of DIY interlace and interlace or not -
             * no interlace + DIY should be identical to no interlace with
             * libpng doing it.
             */
            make_size_image(ps, colour_type, DEPTH(bdlo), PNG_INTERLACE_NONE,
               width, height, 0);
            make_size_image(ps, colour_type, DEPTH(bdlo), PNG_INTERLACE_NONE,
               width, height, 1);

 #        ifdef PNG_WRITE_INTERLACING_SUPPORTED
             make_size_image(ps, colour_type, DEPTH(bdlo), PNG_INTERLACE_ADAM7,
                width, height, 0);
             make_size_image(ps, colour_type, DEPTH(bdlo), PNG_INTERLACE_ADAM7,
                width, height, 1);
 #        endif
 }
 }
 }
}
