 int SoundPool::load(const char* path, int priority __unused)
 {
     ALOGV("load: path=%s, priority=%d", path, priority);
    int sampleID;
    {
        Mutex::Autolock lock(&mLock);
        sampleID = ++mNextSampleID;
        sp<Sample> sample = new Sample(sampleID, path);
        mSamples.add(sampleID, sample);
        sample->startLoad();
    }
    // mDecodeThread->loadSample() must be called outside of mLock.
    // mDecodeThread->loadSample() may block on mDecodeThread message queue space;
    // the message queue emptying may block on SoundPool::findSample().
    //
    // It theoretically possible that sample loads might decode out-of-order.
    mDecodeThread->loadSample(sampleID);
    return sampleID;
 }
