status_t MPEG4Source::fragmentedRead(
 MediaBuffer **out, const ReadOptions *options) {

    ALOGV("MPEG4Source::fragmentedRead");

    CHECK(mStarted);

 *out = NULL;

 int64_t targetSampleTimeUs = -1;

 int64_t seekTimeUs;
 ReadOptions::SeekMode mode;
 if (options && options->getSeekTo(&seekTimeUs, &mode)) {

 int numSidxEntries = mSegments.size();
 if (numSidxEntries != 0) {
 int64_t totalTime = 0;
 off64_t totalOffset = mFirstMoofOffset;
 for (int i = 0; i < numSidxEntries; i++) {
 const SidxEntry *se = &mSegments[i];
 if (totalTime + se->mDurationUs > seekTimeUs) {
 if ((mode == ReadOptions::SEEK_NEXT_SYNC && seekTimeUs > totalTime) ||
 (mode == ReadOptions::SEEK_CLOSEST_SYNC &&
 (seekTimeUs - totalTime) > (totalTime + se->mDurationUs - seekTimeUs))) {
                        totalTime += se->mDurationUs;
                        totalOffset += se->mSize;
 }
 break;
 }
                totalTime += se->mDurationUs;
                totalOffset += se->mSize;
 }
            mCurrentMoofOffset = totalOffset;
            mCurrentSamples.clear();
            mCurrentSampleIndex = 0;
            parseChunk(&totalOffset);
            mCurrentTime = totalTime * mTimescale / 1000000ll;
 } else {
            mCurrentMoofOffset = mFirstMoofOffset;
            mCurrentSamples.clear();
            mCurrentSampleIndex = 0;
 off64_t tmp = mCurrentMoofOffset;
            parseChunk(&tmp);
            mCurrentTime = 0;
 }

 if (mBuffer != NULL) {
            mBuffer->release();
            mBuffer = NULL;
 }

 }

 off64_t offset = 0;
 size_t size = 0;
 uint32_t cts = 0;
 bool isSyncSample = false;
 bool newBuffer = false;
 if (mBuffer == NULL) {
        newBuffer = true;

 if (mCurrentSampleIndex >= mCurrentSamples.size()) {
 if (mNextMoofOffset <= mCurrentMoofOffset) {
 return ERROR_END_OF_STREAM;
 }
 off64_t nextMoof = mNextMoofOffset;
            mCurrentMoofOffset = nextMoof;
            mCurrentSamples.clear();
            mCurrentSampleIndex = 0;
            parseChunk(&nextMoof);
 if (mCurrentSampleIndex >= mCurrentSamples.size()) {
 return ERROR_END_OF_STREAM;
 }
 }

 const Sample *smpl = &mCurrentSamples[mCurrentSampleIndex];
        offset = smpl->offset;
        size = smpl->size;
        cts = mCurrentTime + smpl->compositionOffset;
        mCurrentTime += smpl->duration;
        isSyncSample = (mCurrentSampleIndex == 0); // XXX

 status_t err = mGroup->acquire_buffer(&mBuffer);

 if (err != OK) {
            CHECK(mBuffer == NULL);
            ALOGV("acquire_buffer returned %d", err);
 return err;
 }
 }

 const Sample *smpl = &mCurrentSamples[mCurrentSampleIndex];
 const sp<MetaData> bufmeta = mBuffer->meta_data();
    bufmeta->clear();
 if (smpl->encryptedsizes.size()) {
        bufmeta->setData(kKeyPlainSizes, 0,
                smpl->clearsizes.array(), smpl->clearsizes.size() * 4);
        bufmeta->setData(kKeyEncryptedSizes, 0,
                smpl->encryptedsizes.array(), smpl->encryptedsizes.size() * 4);
        bufmeta->setData(kKeyCryptoIV, 0, smpl->iv, 16); // use 16 or the actual size?
        bufmeta->setInt32(kKeyCryptoDefaultIVSize, mDefaultIVSize);
        bufmeta->setInt32(kKeyCryptoMode, mCryptoMode);
        bufmeta->setData(kKeyCryptoKey, 0, mCryptoKey, 16);
 }

 if ((!mIsAVC && !mIsHEVC)|| mWantsNALFragments) {
 if (newBuffer) {
 ssize_t num_bytes_read =
                mDataSource->readAt(offset, (uint8_t *)mBuffer->data(), size);

 if (num_bytes_read < (ssize_t)size) {
                mBuffer->release();
                mBuffer = NULL;

                ALOGV("i/o error");
 return ERROR_IO;
 }

            CHECK(mBuffer != NULL);
            mBuffer->set_range(0, size);
            mBuffer->meta_data()->setInt64(
                    kKeyTime, ((int64_t)cts * 1000000) / mTimescale);
            mBuffer->meta_data()->setInt64(
                    kKeyDuration, ((int64_t)smpl->duration * 1000000) / mTimescale);

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
        ALOGV("whole NAL");
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

            ALOGV("i/o error");
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

        mBuffer->meta_data()->setInt64(
                kKeyTime, ((int64_t)cts * 1000000) / mTimescale);
        mBuffer->meta_data()->setInt64(
                kKeyDuration, ((int64_t)smpl->duration * 1000000) / mTimescale);

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
