bool SampleTable::isValid() const {

     return mChunkOffsetOffset >= 0
         && mSampleToChunkOffset >= 0
         && mSampleSizeOffset >= 0
        && !mTimeToSample.empty();
 }
