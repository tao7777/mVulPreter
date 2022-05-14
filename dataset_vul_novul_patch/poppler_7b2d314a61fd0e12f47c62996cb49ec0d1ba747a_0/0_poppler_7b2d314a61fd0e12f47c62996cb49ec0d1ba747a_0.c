void CairoOutputDev::beginString(GfxState *state, GooString *s)
{
  int len = s->getLength();

  if (needFontUpdate)
    updateFont(state);

   if (!currentFont)
     return;
 
  glyphs = (cairo_glyph_t *) gmallocn (len, sizeof (cairo_glyph_t));
   glyphCount = 0;
 }
