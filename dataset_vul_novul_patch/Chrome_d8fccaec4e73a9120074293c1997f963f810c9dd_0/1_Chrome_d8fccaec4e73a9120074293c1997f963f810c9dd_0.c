bool HarfBuzzShaper::shape(GlyphBuffer* glyphBuffer)
{
     if (!createHarfBuzzRuns())
         return false;
 
    m_totalWidth = 0;
     if (!shapeHarfBuzzRuns())
         return false;
 
    if (glyphBuffer && !fillGlyphBuffer(glyphBuffer))
        return false;

    return true;
}
