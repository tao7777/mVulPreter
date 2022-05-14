void ID3::Iterator::getstring(String8 *id, bool otherdata) const {
    id->setTo("");

 const uint8_t *frameData = mFrameData;
 if (frameData == NULL) {
 return;
 }

 uint8_t encoding = *frameData;

 if (mParent.mVersion == ID3_V1 || mParent.mVersion == ID3_V1_1) {
 if (mOffset == 126 || mOffset == 127) {
 char tmp[16];
            sprintf(tmp, "%d", (int)*frameData);

            id->setTo(tmp);
 return;
 }

        id->setTo((const char*)frameData, mFrameSize);
 return;
 }

 if (mFrameSize < getHeaderLength() + 1) {
 return;
 }
 size_t n = mFrameSize - getHeaderLength() - 1;
 if (otherdata) {
        frameData += 4;
 int32_t i = n - 4;
 while(--i >= 0 && *++frameData != 0) ;
 int skipped = (frameData - mFrameData);
 if (skipped >= (int)n) {
 return;
 }

         n -= skipped;
     }
 
    if (n <= 0) {
       return;
    }

     if (encoding == 0x00) {
         id->setTo((const char*)frameData + 1, n);
 } else if (encoding == 0x03) {
        id->setTo((const char *)(frameData + 1), n);
 } else if (encoding == 0x02) {
 int len = n / 2;

         const char16_t *framedata = (const char16_t *) (frameData + 1);
         char16_t *framedatacopy = NULL;
 #if BYTE_ORDER == LITTLE_ENDIAN
        if (len > 0) {
            framedatacopy = new (std::nothrow) char16_t[len];
            if (framedatacopy == NULL) {
                return;
            }
            for (int i = 0; i < len; i++) {
                framedatacopy[i] = bswap_16(framedata[i]);
            }
            framedata = framedatacopy;
         }
 #endif
         id->setTo(framedata, len);
         if (framedatacopy != NULL) {
 delete[] framedatacopy;
 }
 } else if (encoding == 0x01) {
 int len = n / 2;

         const char16_t *framedata = (const char16_t *) (frameData + 1);
         char16_t *framedatacopy = NULL;
         if (*framedata == 0xfffe) {
            // endianness marker != host endianness, convert & skip
            if (len <= 1) {
                return;         // nothing after the marker
            }
            framedatacopy = new (std::nothrow) char16_t[len];
            if (framedatacopy == NULL) {
                return;
            }
             for (int i = 0; i < len; i++) {
                 framedatacopy[i] = bswap_16(framedata[i]);
             }
             framedata = framedatacopy;
            // and skip over the marker
            framedata++;
            len--;
        } else if (*framedata == 0xfeff) {
            // endianness marker == host endianness, skip it
            if (len <= 1) {
                return;         // nothing after the marker
            }
             framedata++;
             len--;
         }

 bool eightBit = true;
 for (int i = 0; i < len; i++) {
 if (framedata[i] > 0xff) {
                eightBit = false;
 break;
 }

         }
         if (eightBit) {
            char *frame8 = new (std::nothrow) char[len];
            if (frame8 != NULL) {
                for (int i = 0; i < len; i++) {
                    frame8[i] = framedata[i];
                }
                id->setTo(frame8, len);
                delete [] frame8;
            } else {
                id->setTo(framedata, len);
             }
         } else {
             id->setTo(framedata, len);
         }

 if (framedatacopy != NULL) {
 delete[] framedatacopy;
 }
 }
}
