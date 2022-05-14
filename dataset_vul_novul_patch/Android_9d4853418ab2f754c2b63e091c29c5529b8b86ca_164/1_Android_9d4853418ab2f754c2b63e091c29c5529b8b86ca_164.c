chunk_type_valid(png_uint_32 c)
 /* Bit whacking approach to chunk name validation that is intended to avoid
    * branches.  The cost is that it uses a lot of 32-bit constants, which might
    * be bad on some architectures.
    */
{
   png_uint_32 t;

 /* Remove bit 5 from all but the reserved byte; this means every
    * 8-bit unit must be in the range 65-90 to be valid.  So bit 5
    * must be zero, bit 6 must be set and bit 7 zero.
    */

    c &= ~PNG_U32(32,32,0,32);
    t = (c & ~0x1f1f1f1f) ^ 0x40404040;
 
   /* Subtract 65 for each 8 bit quantity, this must not overflow
     * and each byte must then be in the range 0-25.
     */
    c -= PNG_U32(65,65,65,65);
   t |=c ;

 /* Subtract 26, handling the overflow which should set the top
    * three bits of each byte.
    */
   c -= PNG_U32(25,25,25,26);
   t |= ~c;

 return (t & 0xe0e0e0e0) == 0;
}
