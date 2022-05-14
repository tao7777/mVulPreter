bool CSSPendingAnimations::startPendingAnimations()
{
    bool startedOnCompositor = false;
    for (size_t i = 0; i < m_pending.size(); ++i) {
        if (m_pending[i].first->maybeStartAnimationOnCompositor())
            startedOnCompositor = true;
    }

    if (startedOnCompositor) {
        for (size_t i = 0; i < m_pending.size(); ++i)
             m_waitingForCompositorAnimationStart.append(m_pending[i].first);
     } else {
         for (size_t i = 0; i < m_pending.size(); ++i)
            m_pending[i].first->setStartTime(m_pending[i].second);
     }
     m_pending.clear();
 
    if (startedOnCompositor || m_waitingForCompositorAnimationStart.isEmpty())
        return !m_waitingForCompositorAnimationStart.isEmpty();

    for (size_t i = 0; i < m_waitingForCompositorAnimationStart.size(); ++i) {
        if (m_waitingForCompositorAnimationStart[i].get()->hasActiveAnimationsOnCompositor())
            return true;
    }

    notifyCompositorAnimationStarted(monotonicallyIncreasingTime());
    return false;
}
