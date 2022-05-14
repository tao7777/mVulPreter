status_t AMRSource::read(
 MediaBuffer **out, const ReadOptions *options) {
 *out = NULL;

 
     int64_t seekTimeUs;
     ReadOptions::SeekMode mode;
    if (mOffsetTableLength > 0 && options && options->getSeekTo(&seekTimeUs, &mode)) {
         size_t size;
         int64_t seekFrame = seekTimeUs / 20000ll;  // 20ms per frame.
         mCurrentTimeUs = seekFrame * 20000ll;

 size_t index = seekFrame < 0 ? 0 : seekFrame / 50;
 if (index >= mOffsetTableLength) {
            index = mOffsetTableLength - 1;
 }

        mOffset = mOffsetTable[index] + (mIsWide ? 9 : 6);

 for (size_t i = 0; i< seekFrame - index * 50; i++) {
 status_t err;
 if ((err = getFrameSizeByOffset(mDataSource, mOffset,
                            mIsWide, &size)) != OK) {
 return err;
 }
            mOffset += size;
 }
 }

 uint8_t header;
 ssize_t n = mDataSource->readAt(mOffset, &header, 1);

 if (n < 1) {
 return ERROR_END_OF_STREAM;
 }

 if (header & 0x83) {

        ALOGE("padding bits must be 0, header is 0x%02x", header);

 return ERROR_MALFORMED;
 }

 unsigned FT = (header >> 3) & 0x0f;

 size_t frameSize = getFrameSize(mIsWide, FT);
 if (frameSize == 0) {
 return ERROR_MALFORMED;
 }

 MediaBuffer *buffer;
 status_t err = mGroup->acquire_buffer(&buffer);
 if (err != OK) {
 return err;
 }

    n = mDataSource->readAt(mOffset, buffer->data(), frameSize);

 if (n != (ssize_t)frameSize) {
        buffer->release();
        buffer = NULL;

 if (n < 0) {
 return ERROR_IO;
 } else {
            mOffset += n;
 return ERROR_END_OF_STREAM;
 }
 }

    buffer->set_range(0, frameSize);
    buffer->meta_data()->setInt64(kKeyTime, mCurrentTimeUs);
    buffer->meta_data()->setInt32(kKeyIsSyncFrame, 1);

    mOffset += frameSize;
    mCurrentTimeUs += 20000; // Each frame is 20ms

 *out = buffer;

 return OK;
}
