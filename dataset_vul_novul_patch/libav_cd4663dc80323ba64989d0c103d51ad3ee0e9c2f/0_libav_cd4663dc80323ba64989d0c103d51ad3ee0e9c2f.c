 static int smacker_decode_tree(BitstreamContext *bc, HuffContext *hc,
                                uint32_t prefix, int length)
 {
    if (length > SMKTREE_DECODE_MAX_RECURSION) {
        av_log(NULL, AV_LOG_ERROR, "Maximum tree recursion level exceeded.\n");
        return AVERROR_INVALIDDATA;
    }

     if (!bitstream_read_bit(bc)) { // Leaf
         if(hc->current >= 256){
             av_log(NULL, AV_LOG_ERROR, "Tree size exceeded!\n");
            return AVERROR_INVALIDDATA;
        }
        if(length){
            hc->bits[hc->current] = prefix;
            hc->lengths[hc->current] = length;
        } else {
            hc->bits[hc->current] = 0;
            hc->lengths[hc->current] = 0;
        }
        hc->values[hc->current] = bitstream_read(bc, 8);
        hc->current++;
        if(hc->maxlength < length)
            hc->maxlength = length;
        return 0;
    } else { //Node
        int r;
        length++;
        r = smacker_decode_tree(bc, hc, prefix, length);
        if(r)
            return r;
        return smacker_decode_tree(bc, hc, prefix | (1 << (length - 1)), length);
    }
}
