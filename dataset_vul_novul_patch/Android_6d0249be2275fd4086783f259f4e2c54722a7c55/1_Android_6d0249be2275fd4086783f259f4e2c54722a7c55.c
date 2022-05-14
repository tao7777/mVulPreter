ID3::ID3(const uint8_t *data, size_t size, bool ignoreV1)
 : mIsValid(false),
      mData(NULL),
      mSize(0),

       mFirstFrameOffset(0),
       mVersion(ID3_UNKNOWN),
       mRawSize(0) {
    sp<MemorySource> source = new MemorySource(data, size);
 
     mIsValid = parseV2(source, 0);
 
 if (!mIsValid && !ignoreV1) {
        mIsValid = parseV1(source);
 }
}
