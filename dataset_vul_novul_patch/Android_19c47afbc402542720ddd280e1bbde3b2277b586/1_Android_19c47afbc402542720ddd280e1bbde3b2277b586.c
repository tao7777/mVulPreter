void SoundPool::doLoad(sp<Sample>& sample)
{
    ALOGV("doLoad: loading sample sampleID=%d", sample->sampleID());
    sample->startLoad();
    mDecodeThread->loadSample(sample->sampleID());
 }
