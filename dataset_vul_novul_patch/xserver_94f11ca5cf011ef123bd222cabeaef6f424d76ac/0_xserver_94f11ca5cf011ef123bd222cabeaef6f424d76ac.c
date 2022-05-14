 tbGetBuffer(unsigned size)
 {
    struct textBuffer *tb;
 
    tb = &textBuffer[textBufferIndex];
    textBufferIndex = (textBufferIndex + 1) % NUM_BUFFER;

    if (size > tb->size) {
        free(tb->buffer);
        tb->buffer = xnfalloc(size);
        tb->size = size;
    }
    return tb->buffer;
 }
