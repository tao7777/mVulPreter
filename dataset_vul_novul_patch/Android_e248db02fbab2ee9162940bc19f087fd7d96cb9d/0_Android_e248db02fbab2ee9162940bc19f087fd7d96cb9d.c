status_t DRMSource::read(MediaBuffer **buffer, const ReadOptions *options) {
 Mutex::Autolock autoLock(mDRMLock);
 status_t err;
 if ((err = mOriginalMediaSource->read(buffer, options)) != OK) {
 return err;
 }

 size_t len = (*buffer)->range_length();

 char *src = (char *)(*buffer)->data() + (*buffer)->range_offset();

 DrmBuffer encryptedDrmBuffer(src, len);
 DrmBuffer decryptedDrmBuffer;
    decryptedDrmBuffer.length = len;
    decryptedDrmBuffer.data = new char[len];
 DrmBuffer *pDecryptedDrmBuffer = &decryptedDrmBuffer;

 if ((err = mDrmManagerClient->decrypt(mDecryptHandle, mTrackId,
 &encryptedDrmBuffer, &pDecryptedDrmBuffer)) != NO_ERROR) {

 if (decryptedDrmBuffer.data) {
 delete [] decryptedDrmBuffer.data;
            decryptedDrmBuffer.data = NULL;
 }

 return err;
 }
    CHECK(pDecryptedDrmBuffer == &decryptedDrmBuffer);

 const char *mime;
    CHECK(getFormat()->findCString(kKeyMIMEType, &mime));

 if (!strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_AVC) && !mWantsNALFragments) {
 uint8_t *dstData = (uint8_t*)src;
 size_t srcOffset = 0;
 size_t dstOffset = 0;

        len = decryptedDrmBuffer.length;
 while (srcOffset < len) {
            CHECK(srcOffset + mNALLengthSize <= len);
 size_t nalLength = 0;
 const uint8_t* data = (const uint8_t*)(&decryptedDrmBuffer.data[srcOffset]);

 switch (mNALLengthSize) {
 case 1:
                    nalLength = *data;
 break;
 case 2:
                    nalLength = U16_AT(data);
 break;
 case 3:
                    nalLength = ((size_t)data[0] << 16) | U16_AT(&data[1]);
 break;
 case 4:
                    nalLength = U32_AT(data);
 break;
 default:
                    CHECK(!"Should not be here.");
 break;
 }

            srcOffset += mNALLengthSize;

 size_t end = srcOffset + nalLength;
 if (end > len || end < srcOffset) {
 if (decryptedDrmBuffer.data) {
 delete [] decryptedDrmBuffer.data;
                    decryptedDrmBuffer.data = NULL;
 }

 return ERROR_MALFORMED;
 }

 if (nalLength == 0) {

                 continue;
             }
 
            if (dstOffset > SIZE_MAX - 4 ||
                dstOffset + 4 > SIZE_MAX - nalLength ||
                dstOffset + 4 + nalLength > (*buffer)->size()) {
                (*buffer)->release();
                (*buffer) = NULL;
                if (decryptedDrmBuffer.data) {
                    delete [] decryptedDrmBuffer.data;
                    decryptedDrmBuffer.data = NULL;
                }
                return ERROR_MALFORMED;
            }
 
             dstData[dstOffset++] = 0;
             dstData[dstOffset++] = 0;
            dstData[dstOffset++] = 0;
            dstData[dstOffset++] = 1;
            memcpy(&dstData[dstOffset], &decryptedDrmBuffer.data[srcOffset], nalLength);
            srcOffset += nalLength;
            dstOffset += nalLength;
 }

        CHECK_EQ(srcOffset, len);
 (*buffer)->set_range((*buffer)->range_offset(), dstOffset);

 } else {
        memcpy(src, decryptedDrmBuffer.data, decryptedDrmBuffer.length);
 (*buffer)->set_range((*buffer)->range_offset(), decryptedDrmBuffer.length);
 }

 if (decryptedDrmBuffer.data) {
 delete [] decryptedDrmBuffer.data;
        decryptedDrmBuffer.data = NULL;
 }

 return OK;
}
