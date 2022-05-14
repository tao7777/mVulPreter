int vnc_hextile_send_framebuffer_update(VncState *vs, int x,
                                        int y, int w, int h)
{
     int i, j;
     int has_fg, has_bg;
     uint8_t *last_fg, *last_bg;
 
    last_fg = (uint8_t *) g_malloc(VNC_SERVER_FB_BYTES);
    last_bg = (uint8_t *) g_malloc(VNC_SERVER_FB_BYTES);
     has_fg = has_bg = 0;
     for (j = y; j < (y + h); j += 16) {
         for (i = x; i < (x + w); i += 16) {
        for (i = x; i < (x + w); i += 16) {
            vs->hextile.send_tile(vs, i, j,
                                  MIN(16, x + w - i), MIN(16, y + h - j),
                                  last_bg, last_fg, &has_bg, &has_fg);
        }
    }
    g_free(last_fg);
    g_free(last_bg);

    return 1;
}
 void vnc_hextile_set_pixel_conversion(VncState *vs, int generic)
 {
     if (!generic) {
        switch (VNC_SERVER_FB_BITS) {
        case 8:
            vs->hextile.send_tile = send_hextile_tile_8;
            break;
        case 16:
            vs->hextile.send_tile = send_hextile_tile_16;
            break;
        case 32:
            vs->hextile.send_tile = send_hextile_tile_32;
            break;
         }
     } else {
        switch (VNC_SERVER_FB_BITS) {
        case 8:
            vs->hextile.send_tile = send_hextile_tile_generic_8;
            break;
        case 16:
            vs->hextile.send_tile = send_hextile_tile_generic_16;
            break;
        case 32:
            vs->hextile.send_tile = send_hextile_tile_generic_32;
            break;
         }
     }
 }
}
