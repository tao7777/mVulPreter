void sendTouchEvent(BlackBerry::Platform::TouchEvent::Type type)
{
    BlackBerry::Platform::TouchEvent event;
    event.m_type = type;
    event.m_points.assign(touches.begin(), touches.end());
    BlackBerry::WebKit::DumpRenderTree::currentInstance()->page()->touchEvent(event);

     Vector<BlackBerry::Platform::TouchPoint> t;
 
     for (Vector<BlackBerry::Platform::TouchPoint>::iterator it = touches.begin(); it != touches.end(); ++it) {
        if (it->m_state != BlackBerry::Platform::TouchPoint::TouchReleased) {
            it->m_state = BlackBerry::Platform::TouchPoint::TouchStationary;
             t.append(*it);
         }
     }
    touches = t;
}
