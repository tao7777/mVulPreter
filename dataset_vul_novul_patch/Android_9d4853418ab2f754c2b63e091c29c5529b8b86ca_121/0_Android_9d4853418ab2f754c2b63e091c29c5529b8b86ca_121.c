 random_32(void)
 {
 
   for (;;)
    {
       png_byte mark[4];
       png_uint_32 result;

      store_pool_mark(mark);
      result = png_get_uint_32(mark);

 if (result != 0)
 return result;
 }
}
