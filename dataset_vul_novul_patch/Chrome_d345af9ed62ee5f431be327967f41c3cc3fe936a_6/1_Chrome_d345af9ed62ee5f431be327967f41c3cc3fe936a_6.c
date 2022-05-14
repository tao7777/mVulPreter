void TouchEventHandler::handleTouchPoint(Platform::TouchPoint& point, unsigned modifiers)
 {
     m_webPage->m_inputHandler->setInputModeEnabled();

    bool shiftActive = modifiers & KEYMOD_SHIFT;
     bool altActive = modifiers & KEYMOD_ALT;
     bool ctrlActive = modifiers & KEYMOD_CTRL;
 
    switch (point.m_state) {
     case Platform::TouchPoint::TouchPressed:
         {
            m_webPage->m_inputHandler->clearDidSpellCheckState();

            if (!m_lastFatFingersResult.isValid())
                doFatFingers(point);

            Element* elementUnderFatFinger = m_lastFatFingersResult.nodeAsElementIfApplicable();

             if (m_lastFatFingersResult.isTextInput()) {
                 elementUnderFatFinger = m_lastFatFingersResult.nodeAsElementIfApplicable(FatFingersResult::ShadowContentNotAllowed, true /* shouldUseRootEditableElement */);
                m_shouldRequestSpellCheckOptions = m_webPage->m_inputHandler->shouldRequestSpellCheckingOptionsForPoint(point.m_pos, elementUnderFatFinger, m_spellCheckOptionRequest);
             }
 
             handleFatFingerPressed(shiftActive, altActive, ctrlActive);
            break;
        }
    case Platform::TouchPoint::TouchReleased:
        {

            if (!m_shouldRequestSpellCheckOptions)
                m_webPage->m_inputHandler->processPendingKeyboardVisibilityChange();

            if (m_webPage->m_inputHandler->isInputMode())
                m_webPage->m_inputHandler->notifyClientOfKeyboardVisibilityChange(true);

            m_webPage->m_tapHighlight->hide();

            IntPoint adjustedPoint = m_webPage->mapFromContentsToViewport(m_lastFatFingersResult.adjustedPosition());
            PlatformMouseEvent mouseEvent(adjustedPoint, m_lastScreenPoint, PlatformEvent::MouseReleased, 1, LeftButton, shiftActive, ctrlActive, altActive, TouchScreen);

            m_webPage->handleMouseEvent(mouseEvent);

            if (m_shouldRequestSpellCheckOptions) {
                IntPoint pixelPositionRelativeToViewport = m_webPage->mapToTransformed(adjustedPoint);
                IntSize screenOffset(m_lastScreenPoint - pixelPositionRelativeToViewport);
                m_webPage->m_inputHandler->requestSpellingCheckingOptions(m_spellCheckOptionRequest, screenOffset);
                m_shouldRequestSpellCheckOptions = false;
            }

            m_lastFatFingersResult.reset(); // Reset the fat finger result as its no longer valid when a user's finger is not on the screen.
            break;
        }
    case Platform::TouchPoint::TouchMoved:
        {
             m_webPage->m_inputHandler->clearDidSpellCheckState();
 
            PlatformMouseEvent mouseEvent(point.m_pos, m_lastScreenPoint, PlatformEvent::MouseMoved, 1, LeftButton, shiftActive, ctrlActive, altActive, TouchScreen);
            m_lastScreenPoint = point.m_screenPos;
             m_webPage->handleMouseEvent(mouseEvent);
             break;
         }
    default:
        break;
    }
}
