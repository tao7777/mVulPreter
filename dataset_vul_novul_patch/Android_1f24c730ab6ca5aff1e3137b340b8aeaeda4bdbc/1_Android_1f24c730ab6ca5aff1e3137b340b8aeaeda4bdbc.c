void CameraSource::dataCallbackTimestamp(int64_t timestampUs,
 int32_t msgType __unused, const sp<IMemory> &data) {
    ALOGV("dataCallbackTimestamp: timestamp %lld us", (long long)timestampUs);
 Mutex::Autolock autoLock(mLock);
 if (!mStarted || (mNumFramesReceived == 0 && timestampUs < mStartTimeUs)) {
        ALOGV("Drop frame at %lld/%lld us", (long long)timestampUs, (long long)mStartTimeUs);
        releaseOneRecordingFrame(data);
 return;
 }

 if (skipCurrentFrame(timestampUs)) {
        releaseOneRecordingFrame(data);
 return;
 }

 if (mNumFramesReceived > 0) {
 if (timestampUs <= mLastFrameTimestampUs) {
            ALOGW("Dropping frame with backward timestamp %lld (last %lld)",
 (long long)timestampUs, (long long)mLastFrameTimestampUs);
            releaseOneRecordingFrame(data);
 return;
 }
 if (timestampUs - mLastFrameTimestampUs > mGlitchDurationThresholdUs) {
 ++mNumGlitches;
 }
 }

    mLastFrameTimestampUs = timestampUs;
 if (mNumFramesReceived == 0) {
        mFirstFrameTimeUs = timestampUs;
 if (mStartTimeUs > 0) {
 if (timestampUs < mStartTimeUs) {
                releaseOneRecordingFrame(data);
 return;
 }
            mStartTimeUs = timestampUs - mStartTimeUs;
 }
 }

     ++mNumFramesReceived;
 
     CHECK(data != NULL && data->size() > 0);
     mFramesReceived.push_back(data);
     int64_t timeUs = mStartTimeUs + (timestampUs - mFirstFrameTimeUs);
     mFrameTimes.push_back(timeUs);
    ALOGV("initial delay: %" PRId64 ", current time stamp: %" PRId64,
        mStartTimeUs, timeUs);
    mFrameAvailableCondition.signal();
}
