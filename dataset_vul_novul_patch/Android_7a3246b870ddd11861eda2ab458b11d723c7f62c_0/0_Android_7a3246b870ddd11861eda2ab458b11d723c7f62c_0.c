ID3::getAlbumArt(size_t *length, String8 *mime) const {
 *length = 0;
    mime->setTo("");

 Iterator it(
 *this,
 (mVersion == ID3_V2_3 || mVersion == ID3_V2_4) ? "APIC" : "PIC");

 while (!it.done()) {
 size_t size;
 const uint8_t *data = it.getData(&size);
 if (!data) {
 return NULL;
 }

 
         if (mVersion == ID3_V2_3 || mVersion == ID3_V2_4) {
             uint8_t encoding = data[0];
            size_t consumed = 1;

            // *always* in an 8-bit encoding
            size_t mimeLen = StringSize(&data[consumed], size - consumed, 0x00);
            if (mimeLen > size - consumed) {
                ALOGW("bogus album art size: mime");
                return NULL;
            }
            mime->setTo((const char *)&data[consumed]);
            consumed += mimeLen;
 
 #if 0
            uint8_t picType = data[consumed];
             if (picType != 0x03) {
                 it.next();
 continue;

             }
 #endif
 
            consumed++;
            if (consumed >= size) {
                ALOGW("bogus album art size: pic type");
                 return NULL;
             }
 
            size_t descLen = StringSize(&data[consumed], size - consumed, encoding);
            consumed += descLen;

            if (consumed >= size) {
                ALOGW("bogus album art size: description");
                return NULL;
            }

            *length = size - consumed;

            return &data[consumed];
         } else {
             uint8_t encoding = data[0];
 
            if (size <= 5) {
                return NULL;
            }

             if (!memcmp(&data[1], "PNG", 3)) {
                 mime->setTo("image/png");
             } else if (!memcmp(&data[1], "JPG", 3)) {
                mime->setTo("image/jpeg");
 } else if (!memcmp(&data[1], "-->", 3)) {
                mime->setTo("text/plain");
 } else {
 return NULL;
 }

#if 0
 uint8_t picType = data[4];
 if (picType != 0x03) {
                it.next();
 continue;

             }
 #endif
 
            size_t descLen = StringSize(&data[5], size - 5, encoding);
            if (descLen > size - 5) {
                return NULL;
            }
 
             *length = size - 5 - descLen;
 
 return &data[5 + descLen];
 }
 }

 return NULL;
}
