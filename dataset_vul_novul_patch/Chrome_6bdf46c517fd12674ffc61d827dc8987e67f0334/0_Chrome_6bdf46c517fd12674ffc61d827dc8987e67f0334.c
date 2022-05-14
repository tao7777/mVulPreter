ReverbConvolverStage::ReverbConvolverStage(const float* impulseResponse, size_t, size_t reverbTotalLatency, size_t stageOffset, size_t stageLength,
                                           size_t fftSize, size_t renderPhase, size_t renderSliceSize, ReverbAccumulationBuffer* accumulationBuffer, bool directMode)
    : m_accumulationBuffer(accumulationBuffer)
    , m_accumulationReadIndex(0)
    , m_inputReadIndex(0)
    , m_directMode(directMode)
{
    ASSERT(impulseResponse);
    ASSERT(accumulationBuffer);

    if (!m_directMode) {
        m_fftKernel = adoptPtr(new FFTFrame(fftSize));
         m_fftKernel->doPaddedFFT(impulseResponse + stageOffset, stageLength);
         m_fftConvolver = adoptPtr(new FFTConvolver(fftSize));
     } else {
        ASSERT(!stageOffset);
        ASSERT(stageLength <= fftSize / 2);

         m_directKernel = adoptPtr(new AudioFloatArray(fftSize / 2));
        m_directKernel->copyToRange(impulseResponse, 0, stageLength);
         m_directConvolver = adoptPtr(new DirectConvolver(renderSliceSize));
     }
     m_temporaryBuffer.allocate(renderSliceSize);

    size_t totalDelay = stageOffset + reverbTotalLatency;

    size_t halfSize = fftSize / 2;
    if (!m_directMode) {
        ASSERT(totalDelay >= halfSize);
        if (totalDelay >= halfSize)
            totalDelay -= halfSize;
    }

    int maxPreDelayLength = std::min(halfSize, totalDelay);
    m_preDelayLength = totalDelay > 0 ? renderPhase % maxPreDelayLength : 0;
    if (m_preDelayLength > totalDelay)
        m_preDelayLength = 0;

    m_postDelayLength = totalDelay - m_preDelayLength;
    m_preReadWriteIndex = 0;
    m_framesProcessed = 0; // total frames processed so far

    size_t delayBufferSize = m_preDelayLength < fftSize ? fftSize : m_preDelayLength;
    delayBufferSize = delayBufferSize < renderSliceSize ? renderSliceSize : delayBufferSize;
    m_preDelayBuffer.allocate(delayBufferSize);
}
