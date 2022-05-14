void MediaElementAudioSourceNode::process(size_t numberOfFrames)
{
    AudioBus* outputBus = output(0)->bus();

    if (!mediaElement() || !m_sourceNumberOfChannels || !m_sourceSampleRate) {
        outputBus->zero();
        return;
    }

     MutexTryLocker tryLocker(m_processLock);
     if (tryLocker.locked()) {
         if (AudioSourceProvider* provider = mediaElement()->audioSourceProvider()) {
             if (m_multiChannelResampler.get()) {
                 ASSERT(m_sourceSampleRate != sampleRate());
                 m_multiChannelResampler->process(provider, outputBus, numberOfFrames);
            } else {
                 ASSERT(m_sourceSampleRate == sampleRate());
                 provider->provideInput(outputBus, numberOfFrames);
             }
         } else {
            outputBus->zero();
        }
    } else {
        outputBus->zero();
    }
}
