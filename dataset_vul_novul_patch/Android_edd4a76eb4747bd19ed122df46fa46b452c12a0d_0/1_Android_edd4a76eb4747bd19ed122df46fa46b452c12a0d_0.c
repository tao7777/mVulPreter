status_t SampleTable::setSyncSampleParams(off64_t data_offset, size_t data_size) {
 if (mSyncSampleOffset >= 0 || data_size < 8) {
 return ERROR_MALFORMED;
 }

    mSyncSampleOffset = data_offset;

 uint8_t header[8];
 if (mDataSource->readAt(
                data_offset, header, sizeof(header)) < (ssize_t)sizeof(header)) {
 return ERROR_IO;
 }

 if (U32_AT(header) != 0) {
 return ERROR_MALFORMED;
 }

    mNumSyncSamples = U32_AT(&header[4]);

 if (mNumSyncSamples < 2) {

         ALOGV("Table of sync samples is empty or has only a single entry!");
     }
 
     mSyncSamples = new uint32_t[mNumSyncSamples];
     size_t size = mNumSyncSamples * sizeof(uint32_t);
     if (mDataSource->readAt(mSyncSampleOffset + 8, mSyncSamples, size)
 != (ssize_t)size) {
 return ERROR_IO;
 }

 for (size_t i = 0; i < mNumSyncSamples; ++i) {
        mSyncSamples[i] = ntohl(mSyncSamples[i]) - 1;
 }

 return OK;
}
