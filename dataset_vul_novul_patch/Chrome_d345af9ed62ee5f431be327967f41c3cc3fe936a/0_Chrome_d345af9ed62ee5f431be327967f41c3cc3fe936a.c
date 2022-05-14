 PlatformTouchPoint::PlatformTouchPoint(const BlackBerry::Platform::TouchPoint& point)
    : m_id(point.id())
    , m_screenPos(point.screenPosition())
    // FIXME: We should be calculating a new viewport position from the current scroll
    // position and the documentContentPosition, in case we scrolled since the platform
    // event was created.
    , m_pos(point.documentViewportPosition())
 {
    switch (point.state()) {
     case BlackBerry::Platform::TouchPoint::TouchReleased:
         m_state = TouchReleased;
         break;
    case BlackBerry::Platform::TouchPoint::TouchMoved:
        m_state = TouchMoved;
        break;
    case BlackBerry::Platform::TouchPoint::TouchPressed:
        m_state = TouchPressed;
        break;
    case BlackBerry::Platform::TouchPoint::TouchStationary:
        m_state = TouchStationary;
        break;
    default:
        m_state = TouchStationary; // make sure m_state is initialized
        BLACKBERRY_ASSERT(false);
        break;
    }
}
