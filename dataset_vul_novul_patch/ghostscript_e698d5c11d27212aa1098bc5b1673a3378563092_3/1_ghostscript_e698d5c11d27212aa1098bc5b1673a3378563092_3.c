 jbig2_image_compose(Jbig2Ctx *ctx, Jbig2Image *dst, Jbig2Image *src, int x, int y, Jbig2ComposeOp op)
 {
    int i, j;
    int w, h;
    int leftbyte, rightbyte;
    int shift;
     uint8_t *s, *ss;
     uint8_t *d, *dd;
     uint8_t mask, rightmask;

    if (op != JBIG2_COMPOSE_OR) {
        /* hand off the the general routine */
        return jbig2_image_compose_unopt(ctx, dst, src, x, y, op);
    }

    /* clip */
    w = src->width;
    h = src->height;
    ss = src->data;

    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
         h += y;
         y = 0;
     }
    w = (x + w < dst->width) ? w : dst->width - x;
    h = (y + h < dst->height) ? h : dst->height - y;
 #ifdef JBIG2_DEBUG
     jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, -1, "compositing %dx%d at (%d, %d) after clipping\n", w, h, x, y);
 #endif

    /* check for zero clipping region */
    if ((w <= 0) || (h <= 0)) {
#ifdef JBIG2_DEBUG
        jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, -1, "zero clipping region");
#endif
        return 0;
    }
#if 0
    /* special case complete/strip replacement */
    /* disabled because it's only safe to do when the destination
       buffer is all-blank. */
    if ((x == 0) && (w == src->width)) {
        memcpy(dst->data + y * dst->stride, src->data, h * src->stride);
        return 0;
     }
 #endif
 
    leftbyte = x >> 3;
    rightbyte = (x + w - 1) >> 3;
     shift = x & 7;
 
     /* general OR case */
    s = ss;
    d = dd = dst->data + y * dst->stride + leftbyte;
    if (d < dst->data || leftbyte > dst->stride || h * dst->stride < 0 || d - leftbyte + h * dst->stride > dst->data + dst->height * dst->stride) {
        return jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "preventing heap overflow in jbig2_image_compose");
    }
    if (leftbyte == rightbyte) {
        mask = 0x100 - (0x100 >> w);
        for (j = 0; j < h; j++) {
            *d |= (*s & mask) >> shift;
            d += dst->stride;
            s += src->stride;
        }
    } else if (shift == 0) {
        rightmask = (w & 7) ? 0x100 - (1 << (8 - (w & 7))) : 0xFF;
        for (j = 0; j < h; j++) {
            for (i = leftbyte; i < rightbyte; i++)
                *d++ |= *s++;
            *d |= *s & rightmask;
            d = (dd += dst->stride);
            s = (ss += src->stride);
        }
    } else {
        bool overlap = (((w + 7) >> 3) < ((x + w + 7) >> 3) - (x >> 3));

        mask = 0x100 - (1 << shift);
        if (overlap)
            rightmask = (0x100 - (0x100 >> ((x + w) & 7))) >> (8 - shift);
        else
            rightmask = 0x100 - (0x100 >> (w & 7));
        for (j = 0; j < h; j++) {
            *d++ |= (*s & mask) >> shift;
            for (i = leftbyte; i < rightbyte - 1; i++) {
                *d |= ((*s++ & ~mask) << (8 - shift));
                *d++ |= ((*s & mask) >> shift);
            }
            if (overlap)
                *d |= (*s & rightmask) << (8 - shift);
            else
                *d |= ((s[0] & ~mask) << (8 - shift)) | ((s[1] & rightmask) >> shift);
            d = (dd += dst->stride);
            s = (ss += src->stride);
        }
    }

    return 0;
}
