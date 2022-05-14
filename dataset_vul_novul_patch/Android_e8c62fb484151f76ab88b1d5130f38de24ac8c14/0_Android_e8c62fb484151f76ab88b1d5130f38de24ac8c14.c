 native_handle_t* native_handle_create(int numFds, int numInts)
 {
    if (numFds < 0 || numInts < 0 || numFds > kMaxNativeFds || numInts > kMaxNativeInts) {
        return NULL;
    }
 
    size_t mallocSize = sizeof(native_handle_t) + (sizeof(int) * (numFds + numInts));
    native_handle_t* h = malloc(mallocSize);
     if (h) {
         h->version = sizeof(native_handle_t);
         h->numFds = numFds;
        h->numInts = numInts;
 }
 return h;
}
