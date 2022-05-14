 jbig2_sd_count_referred(Jbig2Ctx *ctx, Jbig2Segment *segment)
 {
     int index;
     Jbig2Segment *rsegment;
    uint32_t n_dicts = 0;
 
     for (index = 0; index < segment->referred_to_segment_count; index++) {
         rsegment = jbig2_find_segment(ctx, segment->referred_to_segments[index]);
        if (rsegment && ((rsegment->flags & 63) == 0) &&
                rsegment->result && (((Jbig2SymbolDict *) rsegment->result)->n_symbols > 0) && ((*((Jbig2SymbolDict *) rsegment->result)->glyphs) != NULL))
            n_dicts++;
    }

    return (n_dicts);
}
