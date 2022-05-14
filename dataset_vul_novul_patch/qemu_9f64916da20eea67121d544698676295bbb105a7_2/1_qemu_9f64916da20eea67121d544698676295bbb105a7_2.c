static int send_solid_rect(VncState *vs)
{
    size_t bytes;
         tight_pack24(vs, vs->tight.tight.buffer, 1, &vs->tight.tight.offset);
         bytes = 3;
     } else {
        bytes = vs->clientds.pf.bytes_per_pixel;
     }
