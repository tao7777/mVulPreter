void Scrollbar::autoscrollPressedPart(double delay)
{
    if (m_pressedPart == ThumbPart || m_pressedPart == NoPart)
        return;

    if ((m_pressedPart == BackTrackPart || m_pressedPart == ForwardTrackPart) && thumbUnderMouse(this)) {
        theme()->invalidatePart(this, m_pressedPart);
        setHoveredPart(ThumbPart);
        return;
     }
 
    if (scrollableArea()->scroll(pressedPartScrollDirection(), pressedPartScrollGranularity()))
         startTimerIfNeeded(delay);
 }
