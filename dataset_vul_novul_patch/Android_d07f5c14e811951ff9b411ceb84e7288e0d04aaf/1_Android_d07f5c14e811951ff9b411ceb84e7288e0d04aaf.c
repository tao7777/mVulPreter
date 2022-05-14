status_t MyOpusExtractor::readNextPacket(MediaBuffer **out) {
 if (mOffset <= mFirstDataOffset && mStartGranulePosition < 0) {
 MediaBuffer *mBuf;
 uint32_t numSamples = 0;
 uint64_t curGranulePosition = 0;
 while (true) {
 status_t err = _readNextPacket(&mBuf, /* calcVorbisTimestamp = */false);
 if (err != OK && err != ERROR_END_OF_STREAM) {
 return err;

             }
             if (err == ERROR_END_OF_STREAM || mCurrentPage.mPageNo > 2) {
                 break;
             }
             curGranulePosition = mCurrentPage.mGranulePosition;
            numSamples += getNumSamplesInPacket(mBuf);
            mBuf->release();
            mBuf = NULL;
 }

 if (curGranulePosition > numSamples) {
            mStartGranulePosition = curGranulePosition - numSamples;
 } else {
            mStartGranulePosition = 0;
 }
        seekToOffset(0);
 }

 status_t err = _readNextPacket(out, /* calcVorbisTimestamp = */false);
 if (err != OK) {
 return err;
 }

 int32_t currentPageSamples;
 if ((*out)->meta_data()->findInt32(kKeyValidSamples, &currentPageSamples)) {
 if (mOffset == mFirstDataOffset) {
            currentPageSamples -= mStartGranulePosition;
 (*out)->meta_data()->setInt32(kKeyValidSamples, currentPageSamples);
 }
        mCurGranulePosition = mCurrentPage.mGranulePosition - currentPageSamples;
 }

 int64_t timeUs = getTimeUsOfGranule(mCurGranulePosition);
 (*out)->meta_data()->setInt64(kKeyTime, timeUs);

 uint32_t frames = getNumSamplesInPacket(*out);
    mCurGranulePosition += frames;
 return OK;
}
