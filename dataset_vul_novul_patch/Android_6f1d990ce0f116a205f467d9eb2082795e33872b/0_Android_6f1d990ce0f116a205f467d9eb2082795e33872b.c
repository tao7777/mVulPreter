bool ID3::removeUnsynchronizationV2_4(bool iTunesHack) {
 size_t oldSize = mSize;

 size_t offset = 0;
 while (mSize >= 10 && offset <= mSize - 10) {
 if (!memcmp(&mData[offset], "\0\0\0\0", 4)) {
 break;
 }

 size_t dataSize;
 if (iTunesHack) {
            dataSize = U32_AT(&mData[offset + 4]);
 } else if (!ParseSyncsafeInteger(&mData[offset + 4], &dataSize)) {
 return false;
 }

 if (dataSize > mSize - 10 - offset) {
 return false;
 }

 uint16_t flags = U16_AT(&mData[offset + 8]);
 uint16_t prevFlags = flags;

 if (flags & 1) {

 if (mSize < 14 || mSize - 14 < offset || dataSize < 4) {
 return false;
 }
            memmove(&mData[offset + 10], &mData[offset + 14], mSize - offset - 14);
            mSize -= 4;
            dataSize -= 4;


             flags &= ~1;
         }
 
        if ((flags & 2) && (dataSize >= 2)) {
 
 size_t readOffset = offset + 11;
 size_t writeOffset = offset + 11;
 for (size_t i = 0; i + 1 < dataSize; ++i) {
 if (mData[readOffset - 1] == 0xff
 && mData[readOffset] == 0x00) {
 ++readOffset;
 --mSize;
 --dataSize;
 }

                 mData[writeOffset++] = mData[readOffset++];
             }
            if (readOffset <= oldSize) {
                memmove(&mData[writeOffset], &mData[readOffset], oldSize - readOffset);
            } else {
                ALOGE("b/34618607 (%zu %zu %zu %zu)", readOffset, writeOffset, oldSize, mSize);
                android_errorWriteLog(0x534e4554, "34618607");
            }
 
         }
        flags &= ~2;
         if (flags != prevFlags || iTunesHack) {
             WriteSyncsafeInteger(&mData[offset + 4], dataSize);
             mData[offset + 8] = flags >> 8;
            mData[offset + 9] = flags & 0xff;
 }

        offset += 10 + dataSize;
 }

    memset(&mData[mSize], 0, oldSize - mSize);

 return true;
}
