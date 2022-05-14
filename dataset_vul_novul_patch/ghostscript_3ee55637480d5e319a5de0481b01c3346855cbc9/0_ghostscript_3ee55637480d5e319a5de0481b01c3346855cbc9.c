 xps_select_font_encoding(xps_font_t *font, int idx)
 {
     byte *cmapdata, *entry;
     int pid, eid;
     if (idx < 0 || idx >= font->cmapsubcount)
        return 0;
     cmapdata = font->data + font->cmaptable;
     entry = cmapdata + 4 + idx * 8;
     pid = u16(entry + 0);
     eid = u16(entry + 2);
     font->cmapsubtable = font->cmaptable + u32(entry + 4);
    if (font->cmapsubtable >= font->length) {
        font->cmapsubtable = 0;
        return 0;
    }
     font->usepua = (pid == 3 && eid == 0);
    return 1;
 }
