void BiquadDSPKernel::getFrequencyResponse(int nFrequencies,
                                           const float* frequencyHz,
                                           float* magResponse,
                                           float* phaseResponse)
{
    bool isGood = nFrequencies > 0 && frequencyHz && magResponse && phaseResponse;
    ASSERT(isGood);
    if (!isGood)
        return;

    Vector<float> frequency(nFrequencies);

    double nyquist = this->nyquist();

     for (int k = 0; k < nFrequencies; ++k)
         frequency[k] = narrowPrecisionToFloat(frequencyHz[k] / nyquist);
 
    double cutoffFrequency;
    double Q;
    double gain;
    double detune; // in Cents

    {
        // Get a copy of the current biquad filter coefficients so we can update the biquad with
        // these values. We need to synchronize with process() to prevent process() from updating
        // the filter coefficients while we're trying to access them. The process will update it
        // next time around.
        //
        // The BiquadDSPKernel object here (along with it's Biquad object) is for querying the
        // frequency response and is NOT the same as the one in process() which is used for
        // performing the actual filtering. This one is is created in
        // BiquadProcessor::getFrequencyResponse for this purpose. Both, however, point to the same
        // BiquadProcessor object.
        //
        // FIXME: Simplify this: crbug.com/390266
        MutexLocker processLocker(m_processLock);

        cutoffFrequency = biquadProcessor()->parameter1()->value();
        Q = biquadProcessor()->parameter2()->value();
        gain = biquadProcessor()->parameter3()->value();
        detune = biquadProcessor()->parameter4()->value();
    }
 
    updateCoefficients(cutoffFrequency, Q, gain, detune);
 
     m_biquad.getFrequencyResponse(nFrequencies, frequency.data(), magResponse, phaseResponse);
 }
