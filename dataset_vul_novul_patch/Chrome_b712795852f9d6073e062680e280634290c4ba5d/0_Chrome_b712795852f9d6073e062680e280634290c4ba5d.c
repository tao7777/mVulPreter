 HarfBuzzShaperBase::HarfBuzzShaperBase(const Font* font, const TextRun& run)
     : m_font(font)
    , m_normalizedBufferLength(0)
     , m_run(run)
     , m_wordSpacingAdjustment(font->wordSpacing())
    , m_padding(0)
    , m_padPerWordBreak(0)
    , m_padError(0)
     , m_letterSpacing(font->letterSpacing())
 {
 }
