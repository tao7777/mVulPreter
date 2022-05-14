void CSSPendingAnimations::notifyCompositorAnimationStarted(double monotonicAnimationStartTime)
 {
     for (size_t i = 0; i < m_waitingForCompositorAnimationStart.size(); ++i) {
         Player* player = m_waitingForCompositorAnimationStart[i].get();
        player->setStartTime(monotonicAnimationStartTime - player->timeline().zeroTime());
     }
 
     m_waitingForCompositorAnimationStart.clear();
}
