 static bool check_solid_tile(VncState *vs, int x, int y, int w, int h,
                              uint32_t* color, bool samecolor)
 {
    switch (VNC_SERVER_FB_BYTES) {
     case 4:
         return check_solid_tile32(vs, x, y, w, h, color, samecolor);
     case 2:
    switch(vd->server->pf.bytes_per_pixel) {
    case 4:
        return check_solid_tile32(vs, x, y, w, h, color, samecolor);
    case 2:
        return check_solid_tile16(vs, x, y, w, h, color, samecolor);
    default:
        return check_solid_tile8(vs, x, y, w, h, color, samecolor);
    }
}

static void find_best_solid_area(VncState *vs, int x, int y, int w, int h,
                                 uint32_t color, int *w_ptr, int *h_ptr)
{
    int dx, dy, dw, dh;
    int w_prev;
    int w_best = 0, h_best = 0;

    w_prev = w;

    for (dy = y; dy < y + h; dy += VNC_TIGHT_MAX_SPLIT_TILE_SIZE) {

        dh = MIN(VNC_TIGHT_MAX_SPLIT_TILE_SIZE, y + h - dy);
        dw = MIN(VNC_TIGHT_MAX_SPLIT_TILE_SIZE, w_prev);

        if (!check_solid_tile(vs, x, dy, dw, dh, &color, true)) {
            break;
        }

        for (dx = x + dw; dx < x + w_prev;) {
            dw = MIN(VNC_TIGHT_MAX_SPLIT_TILE_SIZE, x + w_prev - dx);

            if (!check_solid_tile(vs, dx, dy, dw, dh, &color, true)) {
                break;
            }
            dx += dw;
        }

        w_prev = dx - x;
        if (w_prev * (dy + dh - y) > w_best * h_best) {
            w_best = w_prev;
            h_best = dy + dh - y;
        }
    }

    *w_ptr = w_best;
    *h_ptr = h_best;
}

static void extend_solid_area(VncState *vs, int x, int y, int w, int h,
                              uint32_t color, int *x_ptr, int *y_ptr,
                              int *w_ptr, int *h_ptr)
{
    int cx, cy;

    /* Try to extend the area upwards. */
    for ( cy = *y_ptr - 1;
          cy >= y && check_solid_tile(vs, *x_ptr, cy, *w_ptr, 1, &color, true);
          cy-- );
    *h_ptr += *y_ptr - (cy + 1);
    *y_ptr = cy + 1;

    /* ... downwards. */
    for ( cy = *y_ptr + *h_ptr;
          cy < y + h &&
              check_solid_tile(vs, *x_ptr, cy, *w_ptr, 1, &color, true);
          cy++ );
    *h_ptr += cy - (*y_ptr + *h_ptr);

    /* ... to the left. */
    for ( cx = *x_ptr - 1;
          cx >= x && check_solid_tile(vs, cx, *y_ptr, 1, *h_ptr, &color, true);
          cx-- );
    *w_ptr += *x_ptr - (cx + 1);
    *x_ptr = cx + 1;

    /* ... to the right. */
    for ( cx = *x_ptr + *w_ptr;
          cx < x + w &&
              check_solid_tile(vs, cx, *y_ptr, 1, *h_ptr, &color, true);
          cx++ );
    *w_ptr += cx - (*x_ptr + *w_ptr);
}

static int tight_init_stream(VncState *vs, int stream_id,
                             int level, int strategy)
{
    z_streamp zstream = &vs->tight.stream[stream_id];

    if (zstream->opaque == NULL) {
        int err;

        VNC_DEBUG("VNC: TIGHT: initializing zlib stream %d\n", stream_id);
        VNC_DEBUG("VNC: TIGHT: opaque = %p | vs = %p\n", zstream->opaque, vs);
        zstream->zalloc = vnc_zlib_zalloc;
        zstream->zfree = vnc_zlib_zfree;

        err = deflateInit2(zstream, level, Z_DEFLATED, MAX_WBITS,
                           MAX_MEM_LEVEL, strategy);

        if (err != Z_OK) {
            fprintf(stderr, "VNC: error initializing zlib\n");
            return -1;
        }

        vs->tight.levels[stream_id] = level;
        zstream->opaque = vs;
    }

    if (vs->tight.levels[stream_id] != level) {
        if (deflateParams(zstream, level, strategy) != Z_OK) {
            return -1;
        }
        vs->tight.levels[stream_id] = level;
    }
    return 0;
}

static void tight_send_compact_size(VncState *vs, size_t len)
{
    int lpc = 0;
    int bytes = 0;
    char buf[3] = {0, 0, 0};

    buf[bytes++] = len & 0x7F;
    if (len > 0x7F) {
        buf[bytes-1] |= 0x80;
        buf[bytes++] = (len >> 7) & 0x7F;
        if (len > 0x3FFF) {
            buf[bytes-1] |= 0x80;
            buf[bytes++] = (len >> 14) & 0xFF;
        }
    }
    for (lpc = 0; lpc < bytes; lpc++) {
        vnc_write_u8(vs, buf[lpc]);
    }
}

static int tight_compress_data(VncState *vs, int stream_id, size_t bytes,
                               int level, int strategy)
{
    z_streamp zstream = &vs->tight.stream[stream_id];
    int previous_out;

    if (bytes < VNC_TIGHT_MIN_TO_COMPRESS) {
        vnc_write(vs, vs->tight.tight.buffer, vs->tight.tight.offset);
        return bytes;
    }

    if (tight_init_stream(vs, stream_id, level, strategy)) {
        return -1;
    }

    /* reserve memory in output buffer */
    buffer_reserve(&vs->tight.zlib, bytes + 64);

    /* set pointers */
    zstream->next_in = vs->tight.tight.buffer;
    zstream->avail_in = vs->tight.tight.offset;
    zstream->next_out = vs->tight.zlib.buffer + vs->tight.zlib.offset;
    zstream->avail_out = vs->tight.zlib.capacity - vs->tight.zlib.offset;
    previous_out = zstream->avail_out;
    zstream->data_type = Z_BINARY;

    /* start encoding */
    if (deflate(zstream, Z_SYNC_FLUSH) != Z_OK) {
        fprintf(stderr, "VNC: error during tight compression\n");
        return -1;
    }

    vs->tight.zlib.offset = vs->tight.zlib.capacity - zstream->avail_out;
    /* ...how much data has actually been produced by deflate() */
    bytes = previous_out - zstream->avail_out;

    tight_send_compact_size(vs, bytes);
    vnc_write(vs, vs->tight.zlib.buffer, bytes);

    buffer_reset(&vs->tight.zlib);

    return bytes;
}

/*
 * Subencoding implementations.
 */
static void tight_pack24(VncState *vs, uint8_t *buf, size_t count, size_t *ret)
 
     buf32 = (uint32_t *)buf;
 
    if (1 /* FIXME: (vs->clientds.flags & QEMU_BIG_ENDIAN_FLAG) ==
             (vs->ds->surface->flags & QEMU_BIG_ENDIAN_FLAG) */) {
        rshift = vs->client_pf.rshift;
        gshift = vs->client_pf.gshift;
        bshift = vs->client_pf.bshift;
     } else {
        rshift = 24 - vs->client_pf.rshift;
        gshift = 24 - vs->client_pf.gshift;
        bshift = 24 - vs->client_pf.bshift;
     }
 
     if (ret) {
        bshift = 24 - vs->clientds.pf.bshift;
    }

    if (ret) {
        *ret = count * 3;
    }

    while (count--) {
        pix = *buf32++;
        *buf++ = (char)(pix >> rshift);
        *buf++ = (char)(pix >> gshift);
        *buf++ = (char)(pix >> bshift);
    }
}
