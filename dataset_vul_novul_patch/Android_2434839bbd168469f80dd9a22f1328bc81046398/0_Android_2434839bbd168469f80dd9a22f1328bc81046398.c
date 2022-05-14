status_t SampleTable::setSampleToChunkParams(
 off64_t data_offset, size_t data_size) {
 if (mSampleToChunkOffset >= 0) {
 return ERROR_MALFORMED;
 }

    mSampleToChunkOffset = data_offset;

 if (data_size < 8) {
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

    mNumSampleToChunkOffsets = U32_AT(&header[4]);

 if (data_size < 8 + mNumSampleToChunkOffsets * 12) {

         return ERROR_MALFORMED;
     }
 
    if (SIZE_MAX / sizeof(SampleToChunkEntry) <= mNumSampleToChunkOffsets)
        return ERROR_OUT_OF_RANGE;

     mSampleToChunkEntries =
         new SampleToChunkEntry[mNumSampleToChunkOffsets];
 
 for (uint32_t i = 0; i < mNumSampleToChunkOffsets; ++i) {
 uint8_t buffer[12];
 if (mDataSource->readAt(
                    mSampleToChunkOffset + 8 + i * 12, buffer, sizeof(buffer))
 != (ssize_t)sizeof(buffer)) {
 return ERROR_IO;
 }

        CHECK(U32_AT(buffer) >= 1); // chunk index is 1 based in the spec.

        mSampleToChunkEntries[i].startChunk = U32_AT(buffer) - 1;
        mSampleToChunkEntries[i].samplesPerChunk = U32_AT(&buffer[4]);
        mSampleToChunkEntries[i].chunkDesc = U32_AT(&buffer[8]);
 }

 return OK;
}
