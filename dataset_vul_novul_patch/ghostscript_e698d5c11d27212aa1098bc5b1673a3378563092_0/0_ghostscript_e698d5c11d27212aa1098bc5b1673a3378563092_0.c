jbig2_immediate_generic_region(Jbig2Ctx *ctx, Jbig2Segment *segment, const byte *segment_data)
{
    Jbig2RegionSegmentInfo rsi;
     byte seg_flags;
     int8_t gbat[8];
     int offset;
    uint32_t gbat_bytes = 0;
     Jbig2GenericRegionParams params;
     int code = 0;
     Jbig2Image *image = NULL;
    Jbig2WordStream *ws = NULL;
    Jbig2ArithState *as = NULL;
    Jbig2ArithCx *GB_stats = NULL;

    /* 7.4.6 */
    if (segment->data_length < 18)
                return jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Segment too short");

    jbig2_get_region_segment_info(&rsi, segment_data);
    jbig2_error(ctx, JBIG2_SEVERITY_INFO, segment->number, "generic region: %d x %d @ (%d, %d), flags = %02x", rsi.width, rsi.height, rsi.x, rsi.y, rsi.flags);

    /* 7.4.6.2 */
    seg_flags = segment_data[17];
    jbig2_error(ctx, JBIG2_SEVERITY_INFO, segment->number, "segment flags = %02x", seg_flags);
    if ((seg_flags & 1) && (seg_flags & 6))
        jbig2_error(ctx, JBIG2_SEVERITY_WARNING, segment->number, "MMR is 1, but GBTEMPLATE is not 0");

    /* 7.4.6.3 */
    if (!(seg_flags & 1)) {
        gbat_bytes = (seg_flags & 6) ? 2 : 8;
        if (18 + gbat_bytes > segment->data_length)
            return jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Segment too short");
        memcpy(gbat, segment_data + 18, gbat_bytes);
        jbig2_error(ctx, JBIG2_SEVERITY_INFO, segment->number, "gbat: %d, %d", gbat[0], gbat[1]);
    }

    offset = 18 + gbat_bytes;

    /* Table 34 */
    params.MMR = seg_flags & 1;
    params.GBTEMPLATE = (seg_flags & 6) >> 1;
    params.TPGDON = (seg_flags & 8) >> 3;
    params.USESKIP = 0;
    memcpy(params.gbat, gbat, gbat_bytes);

    image = jbig2_image_new(ctx, rsi.width, rsi.height);
    if (image == NULL)
        return jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "unable to allocate generic image");
    jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, segment->number, "allocated %d x %d image buffer for region decode results", rsi.width, rsi.height);

    if (params.MMR) {
        code = jbig2_decode_generic_mmr(ctx, segment, &params, segment_data + offset, segment->data_length - offset, image);
    } else {
        int stats_size = jbig2_generic_stats_size(ctx, params.GBTEMPLATE);

        GB_stats = jbig2_new(ctx, Jbig2ArithCx, stats_size);
        if (GB_stats == NULL) {
            code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "unable to allocate GB_stats in jbig2_immediate_generic_region");
            goto cleanup;
        }
        memset(GB_stats, 0, stats_size);

        ws = jbig2_word_stream_buf_new(ctx, segment_data + offset, segment->data_length - offset);
        if (ws == NULL) {
            code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "unable to allocate ws in jbig2_immediate_generic_region");
            goto cleanup;
        }
        as = jbig2_arith_new(ctx, ws);
        if (as == NULL) {
            code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "unable to allocate as in jbig2_immediate_generic_region");
            goto cleanup;
        }
        code = jbig2_decode_generic_region(ctx, segment, &params, as, image, GB_stats);
    }

    if (code >= 0)
        jbig2_page_add_result(ctx, &ctx->pages[ctx->current_page], image, rsi.x, rsi.y, rsi.op);
    else
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "error while decoding immediate_generic_region");

cleanup:
    jbig2_free(ctx->allocator, as);
    jbig2_word_stream_buf_free(ctx, ws);
    jbig2_free(ctx->allocator, GB_stats);
    jbig2_image_release(ctx, image);

    return code;
}
