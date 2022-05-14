flx_set_palette_vector (FlxColorSpaceConverter * flxpal, guint start, guint num,
    guchar * newpal, gint scale)
{
  guint grab;

  g_return_if_fail (flxpal != NULL);
  g_return_if_fail (start < 0x100);

  grab = ((start + num) > 0x100 ? 0x100 - start : num);

  if (scale) {
    gint i = 0;

    start *= 3;
    while (grab) {
      flxpal->palvec[start++] = newpal[i++] << scale;
      flxpal->palvec[start++] = newpal[i++] << scale;
      flxpal->palvec[start++] = newpal[i++] << scale;
      grab--;
    }
   } else {
     memcpy (&flxpal->palvec[start * 3], newpal, grab * 3);
   }
 }
