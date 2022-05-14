set_value(png_bytep row, size_t rowbytes, png_uint_32 x, unsigned int bit_depth,
   png_uint_32 value, png_const_bytep gamma_table, double conv)
{
 unsigned int mask = (1U << bit_depth)-1;

   x *= bit_depth; /* Maximum x is 4*1024, maximum bit_depth is 16 */

 if (value <= mask)
 {
      png_uint_32 offset = x >> 3;

 if (offset < rowbytes && (bit_depth < 16 || offset+1 < rowbytes))
 {
         row += offset;

 switch (bit_depth)
 {
 case 1:
 case 2:
 case 4:
 /* Don't gamma correct - values get smashed */
 {
 unsigned int shift = (8 - bit_depth) - (x & 0x7U);

                  mask <<= shift;
                  value = (value << shift) & mask;
 *row = (png_byte)((*row & ~mask) | value);
 }
 return;

 default:
               fprintf(stderr, "makepng: bad bit depth (internal error)\n");

                exit(1);
 
             case 16:
               value = flooru(65535*pow(value/65535.,conv)+.5);
                *row++ = (png_byte)(value >> 8);
                *row = (png_byte)value;
                return;

 case 8:
 *row = gamma_table[value];
 return;
 }
 }

 else
 {
         fprintf(stderr, "makepng: row buffer overflow (internal error)\n");
         exit(1);
 }
 }

 else
 {
      fprintf(stderr, "makepng: component overflow (internal error)\n");
      exit(1);

    }
 }
