void TouchEventHandler::doFatFingers(Platform::TouchPoint& point)
 {
    m_lastScreenPoint = point.m_screenPos;
     m_lastFatFingersResult.reset(); // Theoretically this shouldn't be required. Keep it just in case states get mangled.
    IntPoint contentPos(m_webPage->mapFromViewportToContents(point.m_pos));
     m_webPage->postponeDocumentStyleRecalc();
    m_lastFatFingersResult = FatFingers(m_webPage, contentPos, FatFingers::ClickableElement).findBestPoint();
     m_webPage->resumeDocumentStyleRecalc();
 }
