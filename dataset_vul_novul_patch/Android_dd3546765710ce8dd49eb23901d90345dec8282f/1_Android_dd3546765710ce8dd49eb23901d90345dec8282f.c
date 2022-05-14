AudioSource::AudioSource(
 audio_source_t inputSource, const String16 &opPackageName,
 uint32_t sampleRate, uint32_t channelCount, uint32_t outSampleRate)

     : mStarted(false),
       mSampleRate(sampleRate),
       mOutSampleRate(outSampleRate > 0 ? outSampleRate : sampleRate),
       mPrevSampleTimeUs(0),
       mFirstSampleTimeUs(-1ll),
       mNumFramesReceived(0),
       mNumClientOwnedBuffers(0) {
     ALOGV("sampleRate: %u, outSampleRate: %u, channelCount: %u",
            sampleRate, outSampleRate, channelCount);
    CHECK(channelCount == 1 || channelCount == 2);
    CHECK(sampleRate > 0);

 size_t minFrameCount;
 status_t status = AudioRecord::getMinFrameCount(&minFrameCount,
                                           sampleRate,
                                           AUDIO_FORMAT_PCM_16_BIT,
                                           audio_channel_in_mask_from_count(channelCount));
 if (status == OK) {
 uint32_t frameCount = kMaxBufferSize / sizeof(int16_t) / channelCount;

 size_t bufCount = 2;
 while ((bufCount * frameCount) < minFrameCount) {
            bufCount++;
 }

        mRecord = new AudioRecord(
                    inputSource, sampleRate, AUDIO_FORMAT_PCM_16_BIT,
                    audio_channel_in_mask_from_count(channelCount),
                    opPackageName,
 (size_t) (bufCount * frameCount),
 AudioRecordCallbackFunction,
 this,
                    frameCount /*notificationFrames*/);
        mInitCheck = mRecord->initCheck();
 if (mInitCheck != OK) {
            mRecord.clear();
 }
 } else {
        mInitCheck = status;
 }
}
