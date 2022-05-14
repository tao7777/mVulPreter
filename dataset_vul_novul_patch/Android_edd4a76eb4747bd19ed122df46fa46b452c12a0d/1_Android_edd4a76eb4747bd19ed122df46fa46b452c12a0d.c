status_t SampleTable::setCompositionTimeToSampleParams(
 off64_t data_offset, size_t data_size) {
    ALOGI("There are reordered frames present.");

 if (mCompositionTimeDeltaEntries != NULL || data_size < 8) {
 return ERROR_MALFORMED;
 }

 uint8_t header[8];
 if (mDataSource->readAt(
                data_offset, header, sizeof(header))
 < (ssize_t)sizeof(header)) {
 return ERROR_IO;
 }

 if (U32_AT(header) != 0) {
 return ERROR_MALFORMED;
 }

 size_t numEntries = U32_AT(&header[4]);

 if (data_size != (numEntries + 1) * 8) {
 return ERROR_MALFORMED;

     }
 
     mNumCompositionTimeDeltaEntries = numEntries;
     mCompositionTimeDeltaEntries = new uint32_t[2 * numEntries];
 
     if (mDataSource->readAt(
                data_offset + 8, mCompositionTimeDeltaEntries, numEntries * 8)
 < (ssize_t)numEntries * 8) {
 delete[] mCompositionTimeDeltaEntries;
        mCompositionTimeDeltaEntries = NULL;

 return ERROR_IO;
 }

 for (size_t i = 0; i < 2 * numEntries; ++i) {
        mCompositionTimeDeltaEntries[i] = ntohl(mCompositionTimeDeltaEntries[i]);
 }

    mCompositionDeltaLookup->setEntries(
            mCompositionTimeDeltaEntries, mNumCompositionTimeDeltaEntries);

 return OK;
}
