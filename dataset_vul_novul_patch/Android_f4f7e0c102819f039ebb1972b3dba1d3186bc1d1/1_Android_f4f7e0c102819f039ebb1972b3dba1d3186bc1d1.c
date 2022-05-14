status_t MPEG4Extractor::parse3GPPMetaData(off64_t offset, size_t size, int depth) {
 if (size < 4 || size == SIZE_MAX) {
 return ERROR_MALFORMED;
 }

 uint8_t *buffer = new (std::nothrow) uint8_t[size + 1];
 if (buffer == NULL) {
 return ERROR_MALFORMED;
 }
 if (mDataSource->readAt(
                offset, buffer, size) != (ssize_t)size) {
 delete[] buffer;
        buffer = NULL;

 return ERROR_IO;
 }

 uint32_t metadataKey = 0;
 switch (mPath[depth]) {
 case FOURCC('t', 'i', 't', 'l'):
 {
            metadataKey = kKeyTitle;
 break;
 }
 case FOURCC('p', 'e', 'r', 'f'):
 {
            metadataKey = kKeyArtist;
 break;
 }
 case FOURCC('a', 'u', 't', 'h'):
 {
            metadataKey = kKeyWriter;
 break;
 }
 case FOURCC('g', 'n', 'r', 'e'):
 {
            metadataKey = kKeyGenre;
 break;
 }
 case FOURCC('a', 'l', 'b', 'm'):
 {
 if (buffer[size - 1] != '\0') {
 char tmp[4];
              sprintf(tmp, "%u", buffer[size - 1]);

              mFileMetaData->setCString(kKeyCDTrackNumber, tmp);
 }

            metadataKey = kKeyAlbum;
 break;
 }
 case FOURCC('y', 'r', 'r', 'c'):
 {
 char tmp[5];
 uint16_t year = U16_AT(&buffer[4]);

 if (year < 10000) {
                sprintf(tmp, "%u", year);

                mFileMetaData->setCString(kKeyYear, tmp);
 }
 break;
 }

 default:
 break;
 }

 if (metadataKey > 0) {
 bool isUTF8 = true; // Common case
 char16_t *framedata = NULL;

         int len16 = 0; // Number of UTF-16 characters
 
         if (size - 6 >= 4) {
             len16 = ((size - 6) / 2) - 1; // don't include 0x0000 terminator
             framedata = (char16_t *)(buffer + 6);
 if (0xfffe == *framedata) {
 for (int i = 0; i < len16; i++) {
                    framedata[i] = bswap_16(framedata[i]);
 }
 }

 if (0xfeff == *framedata) {
                framedata++;
                len16--;
                isUTF8 = false;
 }
 }

 if (isUTF8) {
            buffer[size] = 0;
            mFileMetaData->setCString(metadataKey, (const char *)buffer + 6);
 } else {
 String8 tmpUTF8str(framedata, len16);
            mFileMetaData->setCString(metadataKey, tmpUTF8str.string());
 }
 }

 delete[] buffer;
    buffer = NULL;

 return OK;
}
