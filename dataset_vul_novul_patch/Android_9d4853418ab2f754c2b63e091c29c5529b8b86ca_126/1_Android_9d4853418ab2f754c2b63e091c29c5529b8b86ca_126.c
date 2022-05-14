 sample(png_const_bytep row, png_byte colour_type, png_byte bit_depth,
    png_uint_32 x, unsigned int sample_index)
 {
    png_uint_32 bit_index, result;
 
 /* Find a sample index for the desired sample: */
   x *= bit_depth;
   bit_index = x;

 if ((colour_type & 1) == 0) /* !palette */
 {
 if (colour_type & 2)
         bit_index *= 3;

 if (colour_type & 4)
         bit_index += x; /* Alpha channel */

 /* Multiple channels; select one: */
 if (colour_type & (2+4))
         bit_index += sample_index * bit_depth;
 }

 /* Return the sample from the row as an integer. */
   row += bit_index >> 3;
   result = *row;

 if (bit_depth == 8)

       return result;
 
    else if (bit_depth > 8)
      return (result << 8) + *++row;
 
   /* Less than 8 bits per sample. */
    bit_index &= 7;
   return (result >> (8-bit_index-bit_depth)) & ((1U<<bit_depth)-1);
 }
