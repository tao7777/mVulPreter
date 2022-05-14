static void gx_ttfReader__Read(ttfReader *self, void *p, int n)
{
    gx_ttfReader *r = (gx_ttfReader *)self;
    const byte *q;

     if (!r->error) {
         if (r->extra_glyph_index != -1) {
             q = r->glyph_data.bits.data + r->pos;
            r->error = ((r->pos >= r->glyph_data.bits.size ||
                        r->glyph_data.bits.size - r->pos < n) ?
                             gs_note_error(gs_error_invalidfont) : 0);
             if (r->error == 0)
                 memcpy(p, q, n);
            unsigned int cnt;

            for (cnt = 0; cnt < (uint)n; cnt += r->error) {
                r->error = r->pfont->data.string_proc(r->pfont, (ulong)r->pos + cnt, (ulong)n - cnt, &q);
                if (r->error < 0)
                    break;
                else if ( r->error == 0) {
                    memcpy((char *)p + cnt, q, n - cnt);
                    break;
                } else {
                    memcpy((char *)p + cnt, q, r->error);
                }
            }
        }
    }
    if (r->error) {
        memset(p, 0, n);
        return;
    }
    r->pos += n;
}
