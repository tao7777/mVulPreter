pango_glyph_string_set_size (PangoGlyphString *string, gint new_len)
{
  g_return_if_fail (new_len >= 0);

   while (new_len > string->space)
     {
       if (string->space == 0)
	string->space = 1;
       else
	string->space *= 2;
      if (string->space < 0)
 	{
	  g_warning ("glyph string length overflows maximum integer size, truncated");
	  new_len = string->space = G_MAXINT - 8;
 	}
     }
 
  string->glyphs = g_realloc (string->glyphs, string->space * sizeof (PangoGlyphInfo));
  string->log_clusters = g_realloc (string->log_clusters, string->space * sizeof (gint));
  string->num_glyphs = new_len;
}
