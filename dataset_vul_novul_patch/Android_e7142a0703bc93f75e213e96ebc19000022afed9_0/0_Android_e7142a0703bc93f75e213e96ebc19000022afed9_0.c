status_t MPEG4Extractor::readMetaData() {
 if (mInitCheck != NO_INIT) {
 return mInitCheck;
 }

 off64_t offset = 0;
 status_t err;
 bool sawMoovOrSidx = false;

 while (!(sawMoovOrSidx && (mMdatFound || mMoofFound))) {
 off64_t orig_offset = offset;
        err = parseChunk(&offset, 0);

 if (err != OK && err != UNKNOWN_ERROR) {
 break;
 } else if (offset <= orig_offset) {
            ALOGE("did not advance: %lld->%lld", (long long)orig_offset, (long long)offset);
            err = ERROR_MALFORMED;
 break;
 } else if (err == UNKNOWN_ERROR) {
            sawMoovOrSidx = true;
 }
 }

 if (mInitCheck == OK) {
 if (mHasVideo) {
            mFileMetaData->setCString(
                    kKeyMIMEType, MEDIA_MIMETYPE_CONTAINER_MPEG4);
 } else {
            mFileMetaData->setCString(kKeyMIMEType, "audio/mp4");
 }
 } else {
        mInitCheck = err;
 }

    CHECK_NE(err, (status_t)NO_INIT);

 uint64_t psshsize = 0;
 for (size_t i = 0; i < mPssh.size(); i++) {
        psshsize += 20 + mPssh[i].datalen;

     }
     if (psshsize > 0 && psshsize <= UINT32_MAX) {
         char *buf = (char*)malloc(psshsize);
        if (!buf) {
            ALOGE("b/28471206");
            return NO_MEMORY;
        }
         char *ptr = buf;
         for (size_t i = 0; i < mPssh.size(); i++) {
             memcpy(ptr, mPssh[i].uuid, 20); // uuid + length
            memcpy(ptr + 20, mPssh[i].data, mPssh[i].datalen);
            ptr += (20 + mPssh[i].datalen);
 }
        mFileMetaData->setData(kKeyPssh, 'pssh', buf, psshsize);
        free(buf);
 }
 return mInitCheck;
}
