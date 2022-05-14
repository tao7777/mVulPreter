static int send_full_color_rect(VncState *vs, int x, int y, int w, int h)
{
    int stream = 0;
    ssize_t bytes;

#ifdef CONFIG_VNC_PNG
    if (tight_can_send_png_rect(vs, w, h)) {
        return send_png_rect(vs, x, y, w, h, NULL);
    }
#endif
         tight_pack24(vs, vs->tight.tight.buffer, w * h, &vs->tight.tight.offset);
         bytes = 3;
     } else {
        bytes = vs->client_pf.bytes_per_pixel;
     }
