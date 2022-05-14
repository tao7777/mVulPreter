void* H264SwDecMalloc(u32 size) {
void* H264SwDecMalloc(u32 size, u32 num) {
    if (size > UINT32_MAX / num) {
        ALOGE("can't allocate %u * %u bytes", size, num);
        android_errorWriteLog(0x534e4554, "27855419");
        return NULL;
    }
    return malloc(size * num);
 }
