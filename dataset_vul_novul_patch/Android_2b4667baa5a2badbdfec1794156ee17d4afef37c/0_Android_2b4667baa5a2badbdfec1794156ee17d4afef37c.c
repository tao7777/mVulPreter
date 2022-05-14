AMediaCodecCryptoInfo *AMediaCodecCryptoInfo_new(
 int numsubsamples,
 uint8_t key[16],
 uint8_t iv[16],
 cryptoinfo_mode_t mode,
 size_t *clearbytes,

         size_t *encryptedbytes) {
 
    size_t cryptosize;
    // = sizeof(AMediaCodecCryptoInfo) + sizeof(size_t) * numsubsamples * 2;
    if (__builtin_mul_overflow(sizeof(size_t) * 2, numsubsamples, &cryptosize) ||
            __builtin_add_overflow(cryptosize, sizeof(AMediaCodecCryptoInfo), &cryptosize)) {
        ALOGE("crypto size overflow");
        return NULL;
    }
     AMediaCodecCryptoInfo *ret = (AMediaCodecCryptoInfo*) malloc(cryptosize);
     if (!ret) {
         ALOGE("couldn't allocate %zu bytes", cryptosize);
 return NULL;
 }
    ret->numsubsamples = numsubsamples;
    memcpy(ret->key, key, 16);
    memcpy(ret->iv, iv, 16);
    ret->mode = mode;
    ret->pattern.encryptBlocks = 0;
    ret->pattern.skipBlocks = 0;

    ret->clearbytes = (size_t*) (ret + 1); // point immediately after the struct
    ret->encryptedbytes = ret->clearbytes + numsubsamples; // point after the clear sizes

    memcpy(ret->clearbytes, clearbytes, numsubsamples * sizeof(size_t));
    memcpy(ret->encryptedbytes, encryptedbytes, numsubsamples * sizeof(size_t));

 return ret;
}
