status_t MPEG4Source::read(
 MediaBuffer **out, const ReadOptions *options) {
 Mutex::Autolock autoLock(mLock);

    CHECK(mStarted);

 if (mFirstMoofOffset > 0) {
 return fragmentedRead(out, options);
 }

 *out = NULL;

 int64_t targetSampleTimeUs = -1;

 int64_t seekTimeUs;
 ReadOptions::SeekMode mode;
 if (options && options->getSeekTo(&seekTimeUs, &mode)) {
 uint32_t findFlags = 0;
 switch (mode) {
 case ReadOptions::SEEK_PREVIOUS_SYNC:
                findFlags = SampleTable::kFlagBefore;
 break;
 case ReadOptions::SEEK_NEXT_SYNC:
                findFlags = SampleTable::kFlagAfter;
 break;
 case ReadOptions::SEEK_CLOSEST_SYNC:
 case ReadOptions::SEEK_CLOSEST:
                findFlags = SampleTable::kFlagClosest;
 break;
 default:
                CHECK(!"Should not be here.");
 break;
 }

 uint32_t sampleIndex;
 status_t err = mSampleTable->findSampleAtTime(
                seekTimeUs, 1000000, mTimescale,
 &sampleIndex, findFlags);

 if (mode == ReadOptions::SEEK_CLOSEST) {
            findFlags = SampleTable::kFlagBefore;
 }

 uint32_t syncSampleIndex;
 if (err == OK) {
            err = mSampleTable->findSyncSampleNear(
                    sampleIndex, &syncSampleIndex, findFlags);
 }

 uint32_t sampleTime;
 if (err == OK) {
            err = mSampleTable->getMetaDataForSample(
                    sampleIndex, NULL, NULL, &sampleTime);
 }

 if (err != OK) {
 if (err == ERROR_OUT_OF_RANGE) {
                err = ERROR_END_OF_STREAM;
 }
            ALOGV("end of stream");
 return err;
 }

 if (mode == ReadOptions::SEEK_CLOSEST) {
            targetSampleTimeUs = (sampleTime * 1000000ll) / mTimescale;
 }

#if 0
 uint32_t syncSampleTime;
        CHECK_EQ(OK, mSampleTable->getMetaDataForSample(
                    syncSampleIndex, NULL, NULL, &syncSampleTime));

        ALOGI("seek to time %lld us => sample at time %lld us, "
 "sync sample at time %lld us",
             seekTimeUs,
             sampleTime * 1000000ll / mTimescale,
             syncSampleTime * 1000000ll / mTimescale);
#endif

        mCurrentSampleIndex = syncSampleIndex;
 if (mBuffer != NULL) {
            mBuffer->release();
            mBuffer = NULL;
 }

 }

 off64_t offset;
 size_t size;
 uint32_t cts, stts;
 bool isSyncSample;
 bool newBuffer = false;
 if (mBuffer == NULL) {
        newBuffer = true;

 status_t err =
            mSampleTable->getMetaDataForSample(
                    mCurrentSampleIndex, &offset, &size, &cts, &isSyncSample, &stts);

 if (err != OK) {
 return err;
 }

        err = mGroup->acquire_buffer(&mBuffer);

 if (err != OK) {
            CHECK(mBuffer == NULL);
 return err;
 }
 }

 if ((!mIsAVC && !mIsHEVC) || mWantsNALFragments) {
 if (newBuffer) {
 ssize_t num_bytes_read =
                mDataSource->readAt(offset, (uint8_t *)mBuffer->data(), size);

 if (num_bytes_read < (ssize_t)size) {
                mBuffer->release();
                mBuffer = NULL;

 return ERROR_IO;
 }

            CHECK(mBuffer != NULL);
            mBuffer->set_range(0, size);
            mBuffer->meta_data()->clear();
            mBuffer->meta_data()->setInt64(
                    kKeyTime, ((int64_t)cts * 1000000) / mTimescale);
            mBuffer->meta_data()->setInt64(
                    kKeyDuration, ((int64_t)stts * 1000000) / mTimescale);

 if (targetSampleTimeUs >= 0) {
                mBuffer->meta_data()->setInt64(
                        kKeyTargetTime, targetSampleTimeUs);
 }

 if (isSyncSample) {
                mBuffer->meta_data()->setInt32(kKeyIsSyncFrame, 1);
 }

 ++mCurrentSampleIndex;
 }

 if (!mIsAVC && !mIsHEVC) {
 *out = mBuffer;
            mBuffer = NULL;

 return OK;
 }


        CHECK(mBuffer->range_length() >= mNALLengthSize);

 const uint8_t *src =
 (const uint8_t *)mBuffer->data() + mBuffer->range_offset();

 size_t nal_size = parseNALSize(src);
 if (mBuffer->range_length() < mNALLengthSize + nal_size) {
            ALOGE("incomplete NAL unit.");

            mBuffer->release();
            mBuffer = NULL;

 return ERROR_MALFORMED;
 }

 MediaBuffer *clone = mBuffer->clone();
        CHECK(clone != NULL);
        clone->set_range(mBuffer->range_offset() + mNALLengthSize, nal_size);

        CHECK(mBuffer != NULL);
        mBuffer->set_range(
                mBuffer->range_offset() + mNALLengthSize + nal_size,
                mBuffer->range_length() - mNALLengthSize - nal_size);

 if (mBuffer->range_length() == 0) {
            mBuffer->release();
            mBuffer = NULL;
 }

 *out = clone;

 return OK;
 } else {
 ssize_t num_bytes_read = 0;
 int32_t drm = 0;
 bool usesDRM = (mFormat->findInt32(kKeyIsDRM, &drm) && drm != 0);
 if (usesDRM) {
            num_bytes_read =
                mDataSource->readAt(offset, (uint8_t*)mBuffer->data(), size);
 } else {
            num_bytes_read = mDataSource->readAt(offset, mSrcBuffer, size);
 }

 if (num_bytes_read < (ssize_t)size) {
            mBuffer->release();
            mBuffer = NULL;

 return ERROR_IO;
 }

 if (usesDRM) {
            CHECK(mBuffer != NULL);
            mBuffer->set_range(0, size);

 } else {
 uint8_t *dstData = (uint8_t *)mBuffer->data();
 size_t srcOffset = 0;

             size_t dstOffset = 0;
 
             while (srcOffset < size) {
                bool isMalFormed = (srcOffset + mNALLengthSize > size);
                 size_t nalLength = 0;
                 if (!isMalFormed) {
                     nalLength = parseNALSize(&mSrcBuffer[srcOffset]);
                     srcOffset += mNALLengthSize;
                    isMalFormed = srcOffset + nalLength > size;
                 }
 
                 if (isMalFormed) {
                    ALOGE("Video is malformed");
                    mBuffer->release();
                    mBuffer = NULL;
 return ERROR_MALFORMED;
 }

 if (nalLength == 0) {
 continue;
 }

                CHECK(dstOffset + 4 <= mBuffer->size());

                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 1;
                memcpy(&dstData[dstOffset], &mSrcBuffer[srcOffset], nalLength);
                srcOffset += nalLength;
                dstOffset += nalLength;
 }
            CHECK_EQ(srcOffset, size);
            CHECK(mBuffer != NULL);
            mBuffer->set_range(0, dstOffset);
 }

        mBuffer->meta_data()->clear();
        mBuffer->meta_data()->setInt64(
                kKeyTime, ((int64_t)cts * 1000000) / mTimescale);
        mBuffer->meta_data()->setInt64(
                kKeyDuration, ((int64_t)stts * 1000000) / mTimescale);

 if (targetSampleTimeUs >= 0) {
            mBuffer->meta_data()->setInt64(
                    kKeyTargetTime, targetSampleTimeUs);
 }

 if (isSyncSample) {
            mBuffer->meta_data()->setInt32(kKeyIsSyncFrame, 1);
 }

 ++mCurrentSampleIndex;

 *out = mBuffer;
        mBuffer = NULL;

 return OK;
 }
}
