image_size_of_type(int color_type, int bit_depth, unsigned int *colors)
image_size_of_type(int color_type, int bit_depth, unsigned int *colors,
   int small)
 {
    if (*colors)
       return 16;

 else

    {
       int pixel_depth = pixel_depth_of_type(color_type, bit_depth);
 
      if (small)
      {
         if (pixel_depth <= 8) /* there will be one row */
            return 1 << pixel_depth;

         else
            return 256;
      }

      else if (pixel_depth < 8)
          return 64;
 
       else if (pixel_depth > 16)
 return 1024;

 else
 return 256;
 }
}
