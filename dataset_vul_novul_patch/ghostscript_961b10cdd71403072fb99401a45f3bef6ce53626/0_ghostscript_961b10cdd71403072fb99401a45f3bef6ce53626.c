xps_encode_font_char_imp(xps_font_t *font, int code)
{
    byte *table;

    /* no cmap selected: return identity */
    if (font->cmapsubtable <= 0)
        return code;

    table = font->data + font->cmapsubtable;

    switch (u16(table))
    {
    case 0: /* Apple standard 1-to-1 mapping. */
        return table[code + 6];

    case 4: /* Microsoft/Adobe segmented mapping. */
        {
            int segCount2 = u16(table + 6);
            byte *endCount = table + 14;
             byte *startCount = endCount + segCount2 + 2;
             byte *idDelta = startCount + segCount2;
             byte *idRangeOffset = idDelta + segCount2;
            byte *giddata;
             int i2;
 
            if (segCount2 < 3 || segCount2 > 65535 ||
               idRangeOffset > font->data + font->length)
               return gs_error_invalidfont;

           for (i2 = 0; i2 < segCount2 - 3; i2 += 2)
             {
                 int delta, roff;
                 int start = u16(startCount + i2);
                    continue;
                delta = s16(idDelta + i2);
                roff = s16(idRangeOffset + i2);
                if ( roff == 0 )
                {
                    return ( code + delta ) & 0xffff; /* mod 65536 */
                    return 0;
                }
                 if ( roff == 0 )
                 {
                     return ( code + delta ) & 0xffff; /* mod 65536 */
                 }
                if ((giddata = (idRangeOffset + i2 + roff + ((code - start) << 1))) >
                    font->data + font->length) {
                    return code;
                }
                glyph = u16(giddata);
                 return (glyph == 0 ? 0 : glyph + delta);
             }
 
    case 6: /* Single interval lookup. */
        {
            int firstCode = u16(table + 6);
            int entryCount = u16(table + 8);
            if ( code < firstCode || code >= firstCode + entryCount )
                return 0;
            return u16(table + 10 + ((code - firstCode) << 1));
        }

    case 10: /* Trimmed array (like 6) */
        {
            int startCharCode = u32(table + 12);
            int numChars = u32(table + 16);
            if ( code < startCharCode || code >= startCharCode + numChars )
                return 0;
            return u32(table + 20 + (code - startCharCode) * 4);
        }

    case 12: /* Segmented coverage. (like 4) */
        {
            int nGroups = u32(table + 12);
            byte *group = table + 16;
            int i;

            for (i = 0; i < nGroups; i++)
            {
                int startCharCode = u32(group + 0);
                int endCharCode = u32(group + 4);
                int startGlyphID = u32(group + 8);
                if ( code < startCharCode )
                    return 0;
                if ( code <= endCharCode )
                    return startGlyphID + (code - startCharCode);
                group += 12;
            }

            return 0;
        }

    case 2: /* High-byte mapping through table. */
    case 8: /* Mixed 16-bit and 32-bit coverage (like 2) */
    default:
        gs_warn1("unknown cmap format: %d\n", u16(table));
        return 0;
    }

    return 0;
}

/*
 * Given a GID, reverse the CMAP subtable lookup to turn it back into a character code
 * We need a Unicode return value, so we might need to do some fixed tables for
 * certain kinds of CMAP subtables (ie non-Unicode ones). That would be a future enhancement
 * if we ever encounter such a beast.
 */
static int
xps_decode_font_char_imp(xps_font_t *font, int code)
{
    byte *table;

    /* no cmap selected: return identity */
    if (font->cmapsubtable <= 0)
        return code;

    table = font->data + font->cmapsubtable;

    switch (u16(table))
    {
        case 0: /* Apple standard 1-to-1 mapping. */
            {
                int i, length = u16(&table[2]) - 6;

                if (length < 0 || length > 256)
                    return gs_error_invalidfont;

                for (i=0;i<length;i++) {
                    if (table[6 + i] == code)
                        return i;
                }
            }
            return 0;
        case 4: /* Microsoft/Adobe segmented mapping. */
            {
                int segCount2 = u16(table + 6);
                byte *endCount = table + 14;
                byte *startCount = endCount + segCount2 + 2;
                byte *idDelta = startCount + segCount2;
                byte *idRangeOffset = idDelta + segCount2;
                int i2;

                if (segCount2 < 3 || segCount2 > 65535)
                    return gs_error_invalidfont;

                 byte *startCount = endCount + segCount2 + 2;
                 byte *idDelta = startCount + segCount2;
                 byte *idRangeOffset = idDelta + segCount2;
                byte *giddata;
                 int i2;
 
                if (segCount2 < 3 || segCount2 > 65535 ||
                    idRangeOffset > font->data + font->length)
                     return gs_error_invalidfont;
 
                 for (i2 = 0; i2 < segCount2 - 3; i2 += 2)
                        if (roff == 0) {
                            glyph = (i + delta) & 0xffff;
                        } else {
                            glyph = u16(idRangeOffset + i2 + roff + ((i - start) << 1));
                        }
                        if (glyph == code) {
                            return i;
                        }
                    }
                }
                         if (roff == 0) {
                             glyph = (i + delta) & 0xffff;
                         } else {
                            if ((giddata = (idRangeOffset + i2 + roff + ((i - start) << 1))) >
                                 font->data + font->length) {
                                return_error(gs_error_invalidfont);
                            }
                            glyph = u16(giddata);
                         }
                         if (glyph == code) {
                             return i;
                    ch = u16(&table[10 + (i * 2)]);
                    if (ch == code)
                        return (firstCode + i);
                }
            }
            return 0;
        case 10: /* Trimmed array (like 6) */
            {
                unsigned int ch, i, length = u32(&table[20]);
                int firstCode = u32(&table[16]);
                for (i=0;i<length;i++) {
                    ch = u16(&table[10 + (i * 2)]);
                    if (ch == code)
                        return (firstCode + i);
                }
            }
            return 0;
        case 12: /* Segmented coverage. (like 4) */
            {
                unsigned int nGroups = u32(&table[12]);
                int Group;

                for (Group=0;Group<nGroups;Group++)
                {
                    int startCharCode = u32(&table[16 + (Group * 12)]);
                    int endCharCode = u32(&table[16 + (Group * 12) + 4]);
                    int startGlyphCode = u32(&table[16 + (Group * 12) + 8]);

                    if (code >= startGlyphCode && code <= (startGlyphCode + (endCharCode - startCharCode))) {
                        return startGlyphCode + (code - startCharCode);
                    }
                }
            }
            return 0;
        case 2: /* High-byte mapping through table. */
        case 8: /* Mixed 16-bit and 32-bit coverage (like 2) */
        default:
            gs_warn1("unknown cmap format: %d\n", u16(table));
            return 0;
    }
