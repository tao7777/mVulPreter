 double ConvolverNode::latencyTime() const
 {
    return m_reverb ? m_reverb->latencyFrames() / static_cast<double>(sampleRate()) : 0;
 }
