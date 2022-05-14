static inline int hpel_motion(MpegEncContext *s,
                              uint8_t *dest, uint8_t *src,
                              int src_x, int src_y,
                              op_pixels_func *pix_op,
                              int motion_x, int motion_y)
{
    int dxy = 0;
    int emu = 0;

    src_x += motion_x >> 1;
    src_y += motion_y >> 1;

    /* WARNING: do no forget half pels */
    src_x = av_clip(src_x, -16, s->width); // FIXME unneeded for emu?
    if (src_x != s->width)
        dxy |= motion_x & 1;
    src_y = av_clip(src_y, -16, s->height);
    if (src_y != s->height)
         dxy |= (motion_y & 1) << 1;
     src += src_y * s->linesize + src_x;
 
    if ((unsigned)src_x > FFMAX(s->h_edge_pos - (motion_x & 1) - 8, 0) ||
        (unsigned)src_y > FFMAX(s->v_edge_pos - (motion_y & 1) - 8, 0)) {
        s->vdsp.emulated_edge_mc(s->sc.edge_emu_buffer, src,
                                 s->linesize, s->linesize,
                                 9, 9, src_x, src_y,
                                 s->h_edge_pos, s->v_edge_pos);
        src = s->sc.edge_emu_buffer;
        emu = 1;
     }
     pix_op[dxy](dest, src, s->linesize, 8);
     return emu;
    }
