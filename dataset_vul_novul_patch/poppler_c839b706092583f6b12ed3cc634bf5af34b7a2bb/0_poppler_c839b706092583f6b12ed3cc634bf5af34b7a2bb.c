create_surface_from_thumbnail_data (guchar *data,
				    gint    width,
				    gint    height,
 				    gint    rowstride)
 {
   guchar *cairo_pixels;
  gint cairo_stride;
   cairo_surface_t *surface;
   int j;
 
  surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, width, height);
  if (cairo_surface_status (surface))
    return NULL;

  cairo_pixels = cairo_image_surface_get_data (surface);
  cairo_stride = cairo_image_surface_get_stride (surface);
 
   for (j = height; j; j--) {
     guchar *p = data;
     guchar *q = cairo_pixels;
     guchar *end = p + 3 * width;

     while (p < end) {
 #if G_BYTE_ORDER == G_LITTLE_ENDIAN
       q[0] = p[2];
       q[1] = p[1];
       q[2] = p[0];
#else
       q[1] = p[0];
       q[2] = p[1];
       q[3] = p[2];
#endif
      p += 3;
      q += 4;
     }
 
     data += rowstride;
    cairo_pixels += cairo_stride;
   }
 
   return surface;
}
