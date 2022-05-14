huff_get_next_word(Jbig2HuffmanState *hs, int offset)
huff_get_next_word(Jbig2HuffmanState *hs, uint32_t offset)
 {
     uint32_t word = 0;
     Jbig2WordStream *ws = hs->ws;

    if ((ws->get_next_word(ws, offset, &word)) && ((hs->offset_limit == 0) || (offset < hs->offset_limit)))
        hs->offset_limit = offset;
    return word;
}
