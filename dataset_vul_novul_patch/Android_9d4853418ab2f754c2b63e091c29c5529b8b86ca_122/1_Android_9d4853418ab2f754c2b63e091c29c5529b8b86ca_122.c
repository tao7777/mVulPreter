row_copy(png_bytep toBuffer, png_const_bytep fromBuffer, unsigned int bitWidth)
 {
    memcpy(toBuffer, fromBuffer, bitWidth >> 3);
 
 if ((bitWidth & 7) != 0)
 {
 unsigned int mask;

 
       toBuffer += bitWidth >> 3;
       fromBuffer += bitWidth >> 3;
      /* The remaining bits are in the top of the byte, the mask is the bits to
       * retain.
       */
      mask = 0xff >> (bitWidth & 7);
       *toBuffer = (png_byte)((*toBuffer & mask) | (*fromBuffer & ~mask));
    }
 }
