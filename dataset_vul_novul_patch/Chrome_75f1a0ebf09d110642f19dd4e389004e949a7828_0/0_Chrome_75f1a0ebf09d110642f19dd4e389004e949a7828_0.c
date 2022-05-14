void Player::setStartTime(double newStartTime)
void Player::setStartTime(double newStartTime, bool serviceAnimations)
 {
     if (!std::isfinite(newStartTime))
         return;
     updateCurrentTimingState(); // Update the value of held
     m_startTime = newStartTime;
     if (!m_held)
         updateCurrentTimingState();
    if (serviceAnimations)
        m_timeline.serviceAnimations();
    else
        update();
 }
