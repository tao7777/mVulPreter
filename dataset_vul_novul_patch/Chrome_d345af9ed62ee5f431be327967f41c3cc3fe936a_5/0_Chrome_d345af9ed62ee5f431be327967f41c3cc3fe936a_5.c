void TouchEventHandler::doFatFingers(Platform::TouchPoint& point)
void TouchEventHandler::doFatFingers(const Platform::TouchPoint& point)
 {
    m_lastScreenPoint = point.screenPosition();
     m_lastFatFingersResult.reset(); // Theoretically this shouldn't be required. Keep it just in case states get mangled.
     m_webPage->postponeDocumentStyleRecalc();
    m_lastFatFingersResult = FatFingers(m_webPage, point.documentContentPosition(), FatFingers::ClickableElement).findBestPoint();
     m_webPage->resumeDocumentStyleRecalc();
 }
