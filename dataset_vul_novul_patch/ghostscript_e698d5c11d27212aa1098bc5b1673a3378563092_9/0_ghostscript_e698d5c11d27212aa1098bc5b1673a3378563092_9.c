 jbig2_end_of_stripe(Jbig2Ctx *ctx, Jbig2Segment *segment, const uint8_t *segment_data)
 {
     Jbig2Page page = ctx->pages[ctx->current_page];
    uint32_t end_row;
 
    end_row = jbig2_get_uint32(segment_data);
     if (end_row < page.end_row) {
         jbig2_error(ctx, JBIG2_SEVERITY_WARNING, segment->number,
                     "end of stripe segment with non-positive end row advance" " (new end row %d vs current end row %d)", end_row, page.end_row);
    } else {
        jbig2_error(ctx, JBIG2_SEVERITY_INFO, segment->number, "end of stripe: advancing end row to %d", end_row);
    }

    page.end_row = end_row;

    return 0;
}
