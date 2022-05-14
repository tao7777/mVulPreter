HarfBuzzShaper::HarfBuzzShaper(const Font* font, const TextRun& run, const GlyphData* emphasisData,
    HashSet<const SimpleFontData*>* fallbackFonts, FloatRect* bounds)
    : Shaper(font, run, emphasisData, fallbackFonts, bounds)
    , m_normalizedBufferLength(0)
    , m_wordSpacingAdjustment(font->fontDescription().wordSpacing())
    , m_letterSpacing(font->fontDescription().letterSpacing())
     , m_expansionOpportunityCount(0)
     , m_fromIndex(0)
     , m_toIndex(m_run.length())
    , m_totalWidth(0)
 {
     m_normalizedBuffer = adoptArrayPtr(new UChar[m_run.length() + 1]);
     normalizeCharacters(m_run, m_run.length(), m_normalizedBuffer.get(), &m_normalizedBufferLength);
    setExpansion(m_run.expansion());
    setFontFeatures();
}
