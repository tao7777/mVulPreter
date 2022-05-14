void SoundPool::doLoad(sp<Sample>& sample)
    int sampleID;
    {
        Mutex::Autolock lock(&mLock);
        sampleID = ++mNextSampleID;
        sp<Sample> sample = new Sample(sampleID, fd, offset, length);
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
