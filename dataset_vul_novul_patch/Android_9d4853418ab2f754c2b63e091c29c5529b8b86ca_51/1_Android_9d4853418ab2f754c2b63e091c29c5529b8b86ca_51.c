 image_pixel_init(image_pixel *this, png_const_bytep row, png_byte colour_type,
    png_byte bit_depth, png_uint_32 x, store_palette palette)
 {
   PNG_CONST png_byte sample_depth = (png_byte)(colour_type ==
       PNG_COLOR_TYPE_PALETTE ? 8 : bit_depth);
   PNG_CONST unsigned int max = (1U<<sample_depth)-1;
 
    /* Initially just set everything to the same number and the alpha to opaque.
     * Note that this currently assumes a simple palette where entry x has colour
     * rgb(x,x,x)!
     */
    this->palette_index = this->red = this->green = this->blue =
      sample(row, colour_type, bit_depth, x, 0);
    this->alpha = max;
    this->red_sBIT = this->green_sBIT = this->blue_sBIT = this->alpha_sBIT =
       sample_depth;

 /* Then override as appropriate: */
 if (colour_type == 3) /* palette */
 {

       /* This permits the caller to default to the sample value. */
       if (palette != 0)
       {
         PNG_CONST unsigned int i = this->palette_index;
 
          this->red = palette[i].red;
          this->green = palette[i].green;
 this->blue = palette[i].blue;
 this->alpha = palette[i].alpha;
 }
 }

 else /* not palette */

    {
       unsigned int i = 0;
 
       if (colour_type & 2)
       {
         this->green = sample(row, colour_type, bit_depth, x, 1);
         this->blue = sample(row, colour_type, bit_depth, x, 2);
         i = 2;
       }
      if (colour_type & 4)
         this->alpha = sample(row, colour_type, bit_depth, x, ++i);
    }
 
    /* Calculate the scaled values, these are simply the values divided by
     * 'max' and the error is initialized to the double precision epsilon value
     * from the header file.
     */
   image_pixel_setf(this, max);
 
    /* Store the input information for use in the transforms - these will
     * modify the information.
    */
 this->colour_type = colour_type;

    this->bit_depth = bit_depth;
    this->sample_depth = sample_depth;
    this->have_tRNS = 0;
 }
