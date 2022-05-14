void Player::setStartTime(double newStartTime)
 {
     if (!std::isfinite(newStartTime))
         return;
     updateCurrentTimingState(); // Update the value of held
     m_startTime = newStartTime;
     if (!m_held)
         updateCurrentTimingState();
    m_timeline.serviceAnimations();
 }
