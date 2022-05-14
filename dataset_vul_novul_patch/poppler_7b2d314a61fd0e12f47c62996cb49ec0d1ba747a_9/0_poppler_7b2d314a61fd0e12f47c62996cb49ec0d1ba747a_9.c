JBIG2Bitmap::JBIG2Bitmap(Guint segNumA, JBIG2Bitmap *bitmap):
  JBIG2Segment(segNumA)
{
  w = bitmap->w;
  h = bitmap->h;
  line = bitmap->line;

  if (w <= 0 || h <= 0 || line <= 0 || h >= (INT_MAX - 1) / line) {
    error(-1, "invalid width/height");
    data = NULL;
     return;
   }
  data = (Guchar *)gmallocn(h, line + 1);
   memcpy(data, bitmap->data, h * line);
   data[h * line] = 0;
 }
