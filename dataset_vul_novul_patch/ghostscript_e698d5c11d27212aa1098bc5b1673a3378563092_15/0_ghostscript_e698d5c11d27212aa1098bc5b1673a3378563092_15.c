jbig2_sd_list_referred(Jbig2Ctx *ctx, Jbig2Segment *segment)
{
     int index;
     Jbig2Segment *rsegment;
     Jbig2SymbolDict **dicts;
    uint32_t n_dicts = jbig2_sd_count_referred(ctx, segment);
    uint32_t dindex = 0;
 
     dicts = jbig2_new(ctx, Jbig2SymbolDict *, n_dicts);
     if (dicts == NULL) {
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "failed to allocate referred list of symbol dictionaries");
        return NULL;
    }

    for (index = 0; index < segment->referred_to_segment_count; index++) {
        rsegment = jbig2_find_segment(ctx, segment->referred_to_segments[index]);
        if (rsegment && ((rsegment->flags & 63) == 0) && rsegment->result &&
                (((Jbig2SymbolDict *) rsegment->result)->n_symbols > 0) && ((*((Jbig2SymbolDict *) rsegment->result)->glyphs) != NULL)) {
            /* add this referred to symbol dictionary */
            dicts[dindex++] = (Jbig2SymbolDict *) rsegment->result;
        }
    }

    if (dindex != n_dicts) {
        /* should never happen */
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "counted %d symbol dictionaries but built a list with %d.\n", n_dicts, dindex);
    }

    return (dicts);
}
