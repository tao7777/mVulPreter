 void *SoftMP3::memsetSafe(OMX_BUFFERHEADERTYPE *outHeader, int c, size_t len) {
     if (len > outHeader->nAllocLen) {
        ALOGE("memset buffer too small: got %u, expected %zu", outHeader->nAllocLen, len);
         android_errorWriteLog(0x534e4554, "29422022");
         notify(OMX_EventError, OMX_ErrorUndefined, OUTPUT_BUFFER_TOO_SMALL, NULL);
         mSignalledError = true;
 return NULL;
 }
 return memset(outHeader->pBuffer, c, len);
}
