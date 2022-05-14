SampleTable::SampleTable(const sp<DataSource> &source)
 : mDataSource(source),
      mChunkOffsetOffset(-1),
      mChunkOffsetType(0),
      mNumChunkOffsets(0),
      mSampleToChunkOffset(-1),
      mNumSampleToChunkOffsets(0),
      mSampleSizeOffset(-1),

       mSampleSizeFieldSize(0),
       mDefaultSampleSize(0),
       mNumSampleSizes(0),
       mTimeToSampleCount(0),
       mTimeToSample(),
       mSampleTimeEntries(NULL),
      mCompositionTimeDeltaEntries(NULL),
      mNumCompositionTimeDeltaEntries(0),
      mCompositionDeltaLookup(new CompositionDeltaLookup),
      mSyncSampleOffset(-1),
      mNumSyncSamples(0),
      mSyncSamples(NULL),
      mLastSyncSampleIndex(0),
      mSampleToChunkEntries(NULL) {
    mSampleIterator = new SampleIterator(this);
}
