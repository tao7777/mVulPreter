 void LayerWebKitThread::setNeedsCommit()
 {
     if (m_owner)
        m_owner->notifySyncRequired();
 }
