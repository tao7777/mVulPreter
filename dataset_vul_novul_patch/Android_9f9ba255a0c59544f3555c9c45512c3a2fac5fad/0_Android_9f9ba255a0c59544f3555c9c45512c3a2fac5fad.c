bool SoftVPX::outputBufferSafe(OMX_BUFFERHEADERTYPE *outHeader) {
 uint32_t width = outputBufferWidth();
 uint32_t height = outputBufferHeight();

     uint64_t nFilledLen = width;
     nFilledLen *= height;
     if (nFilledLen > UINT32_MAX / 3) {
        ALOGE("b/29421675, nFilledLen overflow %llu w %u h %u",
                (unsigned long long)nFilledLen, width, height);
         android_errorWriteLog(0x534e4554, "29421675");
         return false;
     } else if (outHeader->nAllocLen < outHeader->nFilledLen) {
        ALOGE("b/27597103, buffer too small");
        android_errorWriteLog(0x534e4554, "27597103");
 return false;
 }

 return true;
}
