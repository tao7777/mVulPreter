int SoundPool::play(int sampleID, float leftVolume, float rightVolume,
 int priority, int loop, float rate)

 {
     ALOGV("play sampleID=%d, leftVolume=%f, rightVolume=%f, priority=%d, loop=%d, rate=%f",
             sampleID, leftVolume, rightVolume, priority, loop, rate);
    sp<Sample> sample;
     SoundChannel* channel;
     int channelID;
 
 Mutex::Autolock lock(&mLock);

 if (mQuit) {

         return 0;
     }
    sample = findSample(sampleID);
     if ((sample == 0) || (sample->state() != Sample::READY)) {
         ALOGW("  sample %d not READY", sampleID);
         return 0;
 }

    dump();

    channel = allocateChannel_l(priority);

 if (!channel) {
        ALOGV("No channel allocated");
 return 0;
 }

    channelID = ++mNextChannelID;

    ALOGV("play channel %p state = %d", channel, channel->state());
    channel->play(sample, channelID, leftVolume, rightVolume, priority, loop, rate);
 return channelID;
}
