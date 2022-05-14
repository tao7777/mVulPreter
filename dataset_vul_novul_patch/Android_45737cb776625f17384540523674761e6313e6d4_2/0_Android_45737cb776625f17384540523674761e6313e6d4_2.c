SampleTable::~SampleTable() {
 delete[] mSampleToChunkEntries;
    mSampleToChunkEntries = NULL;

 delete[] mSyncSamples;
    mSyncSamples = NULL;

 delete mCompositionDeltaLookup;
    mCompositionDeltaLookup = NULL;

 delete[] mCompositionTimeDeltaEntries;
    mCompositionTimeDeltaEntries = NULL;


     delete[] mSampleTimeEntries;
     mSampleTimeEntries = NULL;
 
     delete mSampleIterator;
     mSampleIterator = NULL;
 }
