 bool Scrollbar::mouseMoved(const PlatformMouseEvent& evt)
 {
     if (m_pressedPart == ThumbPart) {
        if (theme()->shouldSnapBackToDragOrigin(this, evt)) {
            if (m_scrollableArea)
                m_scrollableArea->scrollToOffsetWithoutAnimation(m_orientation, m_dragOrigin);
        } else {
             moveThumb(m_orientation == HorizontalScrollbar ? 
                       convertFromContainingWindow(evt.pos()).x() :
                       convertFromContainingWindow(evt.pos()).y());
        }
        return true;
    }

    if (m_pressedPart != NoPart)
        m_pressedPos = (orientation() == HorizontalScrollbar ? convertFromContainingWindow(evt.pos()).x() : convertFromContainingWindow(evt.pos()).y());

    ScrollbarPart part = theme()->hitTest(this, evt);    
    if (part != m_hoveredPart) {
        if (m_pressedPart != NoPart) {
            if (part == m_pressedPart) {
                startTimerIfNeeded(theme()->autoscrollTimerDelay());
                theme()->invalidatePart(this, m_pressedPart);
            } else if (m_hoveredPart == m_pressedPart) {
                stopTimerIfNeeded();
                theme()->invalidatePart(this, m_pressedPart);
            }
        } 
        
        setHoveredPart(part);
    } 

    return true;
}
