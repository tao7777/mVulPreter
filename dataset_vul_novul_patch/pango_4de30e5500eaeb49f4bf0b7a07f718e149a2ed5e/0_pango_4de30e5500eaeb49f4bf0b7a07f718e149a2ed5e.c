pango_glyph_string_set_size (PangoGlyphString *string, gint new_len)
{
  g_return_if_fail (new_len >= 0);

   while (new_len > string->space)
     {
       if (string->space == 0)
	{
	  string->space = 4;
	}
       else
 	{
	  const guint max_space =
	    MIN (G_MAXINT, G_MAXSIZE / MAX (sizeof(PangoGlyphInfo), sizeof(gint)));

	  guint more_space = (guint)string->space * 2;

	  if (more_space > max_space)
	    {
	      more_space = max_space;

	      if ((guint)new_len > max_space)
		{
		  g_error ("%s: failed to allocate glyph string of length %i\n",
			   G_STRLOC, new_len);
		}
	    }

	  string->space = more_space;
 	}
     }
 
  string->glyphs = g_realloc (string->glyphs, string->space * sizeof (PangoGlyphInfo));
  string->log_clusters = g_realloc (string->log_clusters, string->space * sizeof (gint));
  string->num_glyphs = new_len;
}
