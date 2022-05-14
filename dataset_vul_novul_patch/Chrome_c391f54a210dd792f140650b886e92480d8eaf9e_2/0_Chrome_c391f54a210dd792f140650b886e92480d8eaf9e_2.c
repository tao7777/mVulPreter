void BiquadDSPKernel::updateCoefficientsIfNecessary(bool useSmoothing, bool forceUpdate)
void BiquadDSPKernel::updateCoefficientsIfNecessary()
 {
    if (biquadProcessor()->filterCoefficientsDirty()) {
        double cutoffFrequency;
        double Q;
         double gain;
         double detune; // in Cents
 
         if (biquadProcessor()->hasSampleAccurateValues()) {
            cutoffFrequency = biquadProcessor()->parameter1()->finalValue();
            Q = biquadProcessor()->parameter2()->finalValue();
             gain = biquadProcessor()->parameter3()->finalValue();
             detune = biquadProcessor()->parameter4()->finalValue();
        } else {
            cutoffFrequency = biquadProcessor()->parameter1()->smoothedValue();
            Q = biquadProcessor()->parameter2()->smoothedValue();
             gain = biquadProcessor()->parameter3()->smoothedValue();
             detune = biquadProcessor()->parameter4()->smoothedValue();
         }
 
        updateCoefficients(cutoffFrequency, Q, gain, detune);
    }
}
 
void BiquadDSPKernel::updateCoefficients(double cutoffFrequency, double Q, double gain, double detune)
{
    // Convert from Hertz to normalized frequency 0 -> 1.
    double nyquist = this->nyquist();
    double normalizedFrequency = cutoffFrequency / nyquist;
 
    // Offset frequency by detune.
    if (detune)
        normalizedFrequency *= pow(2, detune / 1200);
 
    // Configure the biquad with the new filter parameters for the appropriate type of filter.
    switch (biquadProcessor()->type()) {
    case BiquadProcessor::LowPass:
        m_biquad.setLowpassParams(normalizedFrequency, Q);
        break;
 
    case BiquadProcessor::HighPass:
        m_biquad.setHighpassParams(normalizedFrequency, Q);
        break;
 
    case BiquadProcessor::BandPass:
        m_biquad.setBandpassParams(normalizedFrequency, Q);
        break;
 
    case BiquadProcessor::LowShelf:
        m_biquad.setLowShelfParams(normalizedFrequency, gain);
        break;
 
    case BiquadProcessor::HighShelf:
        m_biquad.setHighShelfParams(normalizedFrequency, gain);
        break;
 
    case BiquadProcessor::Peaking:
        m_biquad.setPeakingParams(normalizedFrequency, Q, gain);
        break;
 
    case BiquadProcessor::Notch:
        m_biquad.setNotchParams(normalizedFrequency, Q);
        break;

    case BiquadProcessor::Allpass:
        m_biquad.setAllpassParams(normalizedFrequency, Q);
        break;
     }
 }
