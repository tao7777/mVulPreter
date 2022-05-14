 int SoundPool::load(const char* path, int priority __unused)
 {
     ALOGV("load: path=%s, priority=%d", path, priority);
    Mutex::Autolock lock(&mLock);
    sp<Sample> sample = new Sample(++mNextSampleID, path);
    mSamples.add(sample->sampleID(), sample);
    doLoad(sample);
    return sample->sampleID();
 }
