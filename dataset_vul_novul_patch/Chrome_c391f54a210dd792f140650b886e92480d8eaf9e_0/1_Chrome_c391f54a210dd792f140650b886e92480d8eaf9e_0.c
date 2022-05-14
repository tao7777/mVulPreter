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
 
 
    updateCoefficientsIfNecessary(false, true);
 
     m_biquad.getFrequencyResponse(nFrequencies, frequency.data(), magResponse, phaseResponse);
 }
