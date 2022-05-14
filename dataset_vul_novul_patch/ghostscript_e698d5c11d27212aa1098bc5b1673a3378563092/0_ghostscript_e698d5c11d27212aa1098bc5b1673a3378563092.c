jbig2_word_stream_buf_get_next_word(Jbig2WordStream *self, int offset, uint32_t *word)
jbig2_word_stream_buf_get_next_word(Jbig2WordStream *self, size_t offset, uint32_t *word)
 {
     Jbig2WordStreamBuf *z = (Jbig2WordStreamBuf *) self;
     const byte *data = z->data;
    uint32_t result;

    if (offset + 4 < z->size)
        result = (data[offset] << 24) | (data[offset + 1] << 16) | (data[offset + 2] << 8) | data[offset + 3];
     else if (offset > z->size)
         return -1;
     else {
        size_t i;
 
         result = 0;
         for (i = 0; i < z->size - offset; i++)
            result |= data[offset + i] << ((3 - i) << 3);
    }
    *word = result;
    return 0;
}
