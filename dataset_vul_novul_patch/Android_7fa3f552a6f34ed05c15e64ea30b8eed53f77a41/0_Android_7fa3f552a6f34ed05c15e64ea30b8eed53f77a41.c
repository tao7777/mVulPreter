status_t SampleTable::setSyncSampleParams(off64_t data_offset, size_t data_size) {
 if (mSyncSampleOffset >= 0 || data_size < 8) {
 return ERROR_MALFORMED;
 }

 uint8_t header[8];
 if (mDataSource->readAt(
                data_offset, header, sizeof(header)) < (ssize_t)sizeof(header)) {
 return ERROR_IO;
 }

 if (U32_AT(header) != 0) {
 return ERROR_MALFORMED;
 }

 uint32_t numSyncSamples = U32_AT(&header[4]);

 if (numSyncSamples < 2) {
        ALOGV("Table of sync samples is empty or has only a single entry!");
 }

 uint64_t allocSize = (uint64_t)numSyncSamples * sizeof(uint32_t);
 if (allocSize > kMaxTotalSize) {
        ALOGE("Sync sample table size too large.");
 return ERROR_OUT_OF_RANGE;
 }

    mTotalSize += allocSize;
 if (mTotalSize > kMaxTotalSize) {
        ALOGE("Sync sample table size would make sample table too large.\n"
 "    Requested sync sample table size = %llu\n"
 "    Eventual sample table size >= %llu\n"
 "    Allowed sample table size = %llu\n",
 (unsigned long long)allocSize,
 (unsigned long long)mTotalSize,
 (unsigned long long)kMaxTotalSize);
 return ERROR_OUT_OF_RANGE;
 }

    mSyncSamples = new (std::nothrow) uint32_t[numSyncSamples];
 if (!mSyncSamples) {
        ALOGE("Cannot allocate sync sample table with %llu entries.",
 (unsigned long long)numSyncSamples);
 return ERROR_OUT_OF_RANGE;
 }

 
     if (mDataSource->readAt(data_offset + 8, mSyncSamples,
             (size_t)allocSize) != (ssize_t)allocSize) {
        delete[] mSyncSamples;
         mSyncSamples = NULL;
         return ERROR_IO;
     }

 for (size_t i = 0; i < numSyncSamples; ++i) {
 if (mSyncSamples[i] == 0) {
            ALOGE("b/32423862, unexpected zero value in stss");
 continue;
 }
        mSyncSamples[i] = ntohl(mSyncSamples[i]) - 1;
 }

    mSyncSampleOffset = data_offset;
    mNumSyncSamples = numSyncSamples;

 return OK;
}
