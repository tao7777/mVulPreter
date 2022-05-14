 HarfBuzzShaperBase::HarfBuzzShaperBase(const Font* font, const TextRun& run)
     : m_font(font)
     , m_run(run)
     , m_wordSpacingAdjustment(font->wordSpacing())
     , m_letterSpacing(font->letterSpacing())
 {
 }
