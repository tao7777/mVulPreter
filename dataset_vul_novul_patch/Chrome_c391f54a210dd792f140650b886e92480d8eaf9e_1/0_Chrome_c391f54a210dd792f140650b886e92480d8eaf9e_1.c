void BiquadDSPKernel::process(const float* source, float* destination, size_t framesToProcess)
{
    ASSERT(source && destination && biquadProcessor());

 

    // The audio thread can't block on this lock; skip updating the coefficients for this block if
    // necessary. We'll get them the next time around.
    {
        MutexTryLocker tryLocker(m_processLock);
        if (tryLocker.locked())
            updateCoefficientsIfNecessary();
    }
 
     m_biquad.process(source, destination, framesToProcess);
 }
