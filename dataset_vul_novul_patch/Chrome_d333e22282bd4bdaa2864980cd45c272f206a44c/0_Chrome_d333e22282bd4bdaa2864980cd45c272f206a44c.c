 void LayerWebKitThread::setNeedsCommit()
 {
    // Call notifyFlushRequired(), which in this implementation plumbs through to
     if (m_owner)
        m_owner->notifyFlushRequired();
 }
