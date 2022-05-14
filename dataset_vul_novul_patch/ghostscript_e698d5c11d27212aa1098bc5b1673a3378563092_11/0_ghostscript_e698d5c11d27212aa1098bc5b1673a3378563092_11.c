jbig2_parse_segment_header(Jbig2Ctx *ctx, uint8_t *buf, size_t buf_size, size_t *p_header_size)
{
    Jbig2Segment *result;
     uint8_t rtscarf;
     uint32_t rtscarf_long;
     uint32_t *referred_to_segments;
    uint32_t referred_to_segment_count;
    uint32_t referred_to_segment_size;
    uint32_t pa_size;
    uint32_t offset;
 
     /* minimum possible size of a jbig2 segment header */
     if (buf_size < 11)
        return NULL;

    result = jbig2_new(ctx, Jbig2Segment, 1);
    if (result == NULL) {
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "failed to allocate segment in jbig2_parse_segment_header");
        return result;
    }

    /* 7.2.2 */
    result->number = jbig2_get_uint32(buf);

    /* 7.2.3 */
    result->flags = buf[4];

    /* 7.2.4 referred-to segments */
    rtscarf = buf[5];
    if ((rtscarf & 0xe0) == 0xe0) {
        rtscarf_long = jbig2_get_uint32(buf + 5);
        referred_to_segment_count = rtscarf_long & 0x1fffffff;
        offset = 5 + 4 + (referred_to_segment_count + 1) / 8;
    } else {
        referred_to_segment_count = (rtscarf >> 5);
        offset = 5 + 1;
    }
    result->referred_to_segment_count = referred_to_segment_count;

    /* we now have enough information to compute the full header length */
    referred_to_segment_size = result->number <= 256 ? 1 : result->number <= 65536 ? 2 : 4;     /* 7.2.5 */
    pa_size = result->flags & 0x40 ? 4 : 1;     /* 7.2.6 */
    if (offset + referred_to_segment_count * referred_to_segment_size + pa_size + 4 > buf_size) {
        jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, result->number, "jbig2_parse_segment_header() called with insufficient data", -1);
        jbig2_free(ctx->allocator, result);
        return NULL;
    }
 
     /* 7.2.5 */
     if (referred_to_segment_count) {
        uint32_t i;
 
         referred_to_segments = jbig2_new(ctx, uint32_t, referred_to_segment_count * referred_to_segment_size);
         if (referred_to_segments == NULL) {
            jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "could not allocate referred_to_segments " "in jbig2_parse_segment_header");
            return NULL;
        }

        for (i = 0; i < referred_to_segment_count; i++) {
            referred_to_segments[i] =
                (referred_to_segment_size == 1) ? buf[offset] :
                (referred_to_segment_size == 2) ? jbig2_get_uint16(buf + offset) : jbig2_get_uint32(buf + offset);
            offset += referred_to_segment_size;
            jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, result->number, "segment %d refers to segment %d", result->number, referred_to_segments[i]);
        }
        result->referred_to_segments = referred_to_segments;
    } else {                    /* no referred-to segments */

        result->referred_to_segments = NULL;
    }

    /* 7.2.6 */
    if (result->flags & 0x40) {
        result->page_association = jbig2_get_uint32(buf + offset);
        offset += 4;
    } else {
        result->page_association = buf[offset++];
    }
    jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, result->number, "segment %d is associated with page %d", result->number, result->page_association);

    /* 7.2.7 */
    result->data_length = jbig2_get_uint32(buf + offset);
    *p_header_size = offset + 4;

    /* no body parsing results yet */
    result->result = NULL;

    return result;
}
