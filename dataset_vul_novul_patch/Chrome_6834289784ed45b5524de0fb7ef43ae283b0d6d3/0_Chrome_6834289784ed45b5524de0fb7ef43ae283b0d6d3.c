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
            // Grab data from the provider so that the element continues to make progress, even if
            // we're going to output silence anyway.
             if (m_multiChannelResampler.get()) {
                 ASSERT(m_sourceSampleRate != sampleRate());
                 m_multiChannelResampler->process(provider, outputBus, numberOfFrames);
            } else {
                 ASSERT(m_sourceSampleRate == sampleRate());
                 provider->provideInput(outputBus, numberOfFrames);
             }
            // Output silence if we don't have access to the element.
            if (!(mediaElement()->webMediaPlayer()->didPassCORSAccessCheck()
                || context()->securityOrigin()->canRequest(mediaElement()->currentSrc()))) {
                outputBus->zero();
            }
         } else {
            outputBus->zero();
        }
    } else {
        outputBus->zero();
    }
}
