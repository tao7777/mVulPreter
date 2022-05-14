create_bits (pixman_format_code_t format,
             int                  width,
             int                  height,
             int *		  rowstride_bytes,
	     pixman_bool_t	  clear)
{
    int stride;
    size_t buf_size;
    int bpp;

    /* what follows is a long-winded way, avoiding any possibility of integer
     * overflows, of saying:
     * stride = ((width * bpp + 0x1f) >> 5) * sizeof (uint32_t);
     */

    bpp = PIXMAN_FORMAT_BPP (format);
    if (_pixman_multiply_overflows_int (width, bpp))
	return NULL;

    stride = width * bpp;
    if (_pixman_addition_overflows_int (stride, 0x1f))
	return NULL;

    stride += 0x1f;
    stride >>= 5;

    stride *= sizeof (uint32_t);

     if (_pixman_multiply_overflows_size (height, stride))
 	return NULL;
 
    buf_size = (size_t)height * stride;
 
     if (rowstride_bytes)
 	*rowstride_bytes = stride;

    if (clear)
	return calloc (buf_size, 1);
    else
	return malloc (buf_size);
}
