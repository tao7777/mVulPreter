void BiquadDSPKernel::updateCoefficientsIfNecessary(bool useSmoothing, bool forceUpdate)
 {
    if (forceUpdate || biquadProcessor()->filterCoefficientsDirty()) {
        double value1;
        double value2;
         double gain;
         double detune; // in Cents
 
         if (biquadProcessor()->hasSampleAccurateValues()) {
            value1 = biquadProcessor()->parameter1()->finalValue();
            value2 = biquadProcessor()->parameter2()->finalValue();
             gain = biquadProcessor()->parameter3()->finalValue();
             detune = biquadProcessor()->parameter4()->finalValue();
        } else if (useSmoothing) {
            value1 = biquadProcessor()->parameter1()->smoothedValue();
            value2 = biquadProcessor()->parameter2()->smoothedValue();
             gain = biquadProcessor()->parameter3()->smoothedValue();
             detune = biquadProcessor()->parameter4()->smoothedValue();
        } else {
            value1 = biquadProcessor()->parameter1()->value();
            value2 = biquadProcessor()->parameter2()->value();
            gain = biquadProcessor()->parameter3()->value();
            detune = biquadProcessor()->parameter4()->value();
         }
 
        double nyquist = this->nyquist();
        double normalizedFrequency = value1 / nyquist;
 
        if (detune)
            normalizedFrequency *= pow(2, detune / 1200);
 
        switch (biquadProcessor()->type()) {
        case BiquadProcessor::LowPass:
            m_biquad.setLowpassParams(normalizedFrequency, value2);
            break;
 
        case BiquadProcessor::HighPass:
            m_biquad.setHighpassParams(normalizedFrequency, value2);
            break;
 
        case BiquadProcessor::BandPass:
            m_biquad.setBandpassParams(normalizedFrequency, value2);
            break;
 
        case BiquadProcessor::LowShelf:
            m_biquad.setLowShelfParams(normalizedFrequency, gain);
            break;
 
        case BiquadProcessor::HighShelf:
            m_biquad.setHighShelfParams(normalizedFrequency, gain);
            break;
 
        case BiquadProcessor::Peaking:
            m_biquad.setPeakingParams(normalizedFrequency, value2, gain);
            break;
 
        case BiquadProcessor::Notch:
            m_biquad.setNotchParams(normalizedFrequency, value2);
            break;
 
        case BiquadProcessor::Allpass:
            m_biquad.setAllpassParams(normalizedFrequency, value2);
            break;
        }
     }
 }
