void Scrollbar::moveThumb(int pos)
{
    int thumbPos = theme()->thumbPosition(this);
    int thumbLen = theme()->thumbLength(this);
    int trackLen = theme()->trackLength(this);
    int maxPos = trackLen - thumbLen;
    int delta = pos - m_pressedPos;
    if (delta > 0)
        delta = min(maxPos - thumbPos, delta);
    else if (delta < 0)
        delta = max(-thumbPos, delta);
     
     if (delta) {
         float newPosition = static_cast<float>(thumbPos + delta) * maximum() / (trackLen - thumbLen);
        scrollableArea()->scrollToOffsetWithoutAnimation(m_orientation, newPosition);
     }
 }
