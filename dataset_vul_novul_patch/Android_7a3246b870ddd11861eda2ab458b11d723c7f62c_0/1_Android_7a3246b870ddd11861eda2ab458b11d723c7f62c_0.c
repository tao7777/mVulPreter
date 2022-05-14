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
            mime->setTo((const char *)&data[1]);
            size_t mimeLen = strlen((const char *)&data[1]) + 1;
 
 #if 0
            uint8_t picType = data[1 + mimeLen];
             if (picType != 0x03) {
                 it.next();
 continue;

             }
 #endif
 
            size_t descLen = StringSize(&data[2 + mimeLen], encoding);
            if (size < 2 ||
                    size - 2 < mimeLen ||
                    size - 2 - mimeLen < descLen) {
                ALOGW("bogus album art sizes");
                 return NULL;
             }
            *length = size - 2 - mimeLen - descLen;
 
            return &data[2 + mimeLen + descLen];
         } else {
             uint8_t encoding = data[0];
 
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
 
            size_t descLen = StringSize(&data[5], encoding);
 
             *length = size - 5 - descLen;
 
 return &data[5 + descLen];
 }
 }

 return NULL;
}
