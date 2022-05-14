 double ConvolverNode::latencyTime() const
 {
    MutexTryLocker tryLocker(m_processLock);
    if (tryLocker.locked())
        return m_reverb ? m_reverb->latencyFrames() / static_cast<double>(sampleRate()) : 0;
    // Since we don't want to block the Audio Device thread, we return a large value
    // instead of trying to acquire the lock.
    return std::numeric_limits<double>::infinity();
 }
