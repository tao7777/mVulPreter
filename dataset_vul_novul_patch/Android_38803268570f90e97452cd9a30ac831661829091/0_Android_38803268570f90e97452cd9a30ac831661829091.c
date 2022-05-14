status_t GraphicBuffer::unflatten(
 void const*& buffer, size_t& size, int const*& fds, size_t& count) {
 if (size < 8*sizeof(int)) return NO_MEMORY;

 int const* buf = static_cast<int const*>(buffer);
 if (buf[0] != 'GBFR') return BAD_TYPE;


     const size_t numFds  = buf[8];
     const size_t numInts = buf[9];
 
    const size_t maxNumber = UINT_MAX / sizeof(int);
    if (numFds >= maxNumber || numInts >= (maxNumber - 10)) {
        width = height = stride = format = usage = 0;
        handle = NULL;
        ALOGE("unflatten: numFds or numInts is too large: %d, %d",
                numFds, numInts);
        return BAD_VALUE;
    }

     const size_t sizeNeeded = (10 + numInts) * sizeof(int);
     if (size < sizeNeeded) return NO_MEMORY;
 
    size_t fdCountNeeded = numFds;
     if (count < fdCountNeeded) return NO_MEMORY;
 
     if (handle) {
        free_handle();
 }

 if (numFds || numInts) {
        width  = buf[1];
        height = buf[2];
        stride = buf[3];

         format = buf[4];
         usage  = buf[5];
         native_handle* h = native_handle_create(numFds, numInts);
        if (!h) {
            width = height = stride = format = usage = 0;
            handle = NULL;
            ALOGE("unflatten: native_handle_create failed");
            return NO_MEMORY;
        }
         memcpy(h->data,          fds,     numFds*sizeof(int));
         memcpy(h->data + numFds, &buf[10], numInts*sizeof(int));
         handle = h;
 } else {
        width = height = stride = format = usage = 0;
        handle = NULL;
 }

    mId = static_cast<uint64_t>(buf[6]) << 32;
    mId |= static_cast<uint32_t>(buf[7]);

    mOwner = ownHandle;

 if (handle != 0) {
 status_t err = mBufferMapper.registerBuffer(handle);
 if (err != NO_ERROR) {
            width = height = stride = format = usage = 0;
            handle = NULL;
            ALOGE("unflatten: registerBuffer failed: %s (%d)",
                    strerror(-err), err);
 return err;
 }
 }

    buffer = reinterpret_cast<void const*>(static_cast<int const*>(buffer) + sizeNeeded);
    size -= sizeNeeded;
    fds += numFds;
    count -= numFds;

 return NO_ERROR;
}
