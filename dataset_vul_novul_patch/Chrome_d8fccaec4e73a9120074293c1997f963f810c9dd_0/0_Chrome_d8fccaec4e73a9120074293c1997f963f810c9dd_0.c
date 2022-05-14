bool HarfBuzzShaper::shape(GlyphBuffer* glyphBuffer)
{
     if (!createHarfBuzzRuns())
         return false;
 
     if (!shapeHarfBuzzRuns())
         return false;
 
    if (glyphBuffer && !fillGlyphBuffer(glyphBuffer))
        return false;

    return true;
}
