 jbig2_decode_mmr_init(Jbig2MmrCtx *mmr, int width, int height, const byte *data, size_t size)
 {
    size_t i;
     uint32_t word = 0;
 
     mmr->width = width;
    mmr->size = size;
    mmr->data_index = 0;
    mmr->bit_index = 0;

    for (i = 0; i < size && i < 4; i++)
        word |= (data[i] << ((3 - i) << 3));
    mmr->word = word;
}
