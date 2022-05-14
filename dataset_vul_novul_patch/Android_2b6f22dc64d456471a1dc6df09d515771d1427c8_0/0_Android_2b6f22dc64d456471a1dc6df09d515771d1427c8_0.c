void* H264SwDecMalloc(u32 size)
void* H264SwDecMalloc(u32 size, u32 num)
 {
    if (size > UINT32_MAX / num) {
        return NULL;
    }
    return malloc(size * num);
 }
