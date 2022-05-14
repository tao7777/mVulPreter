void BiquadDSPKernel::process(const float* source, float* destination, size_t framesToProcess)
{
    ASSERT(source && destination && biquadProcessor());

 
    updateCoefficientsIfNecessary(true, false);
 
     m_biquad.process(source, destination, framesToProcess);
 }
