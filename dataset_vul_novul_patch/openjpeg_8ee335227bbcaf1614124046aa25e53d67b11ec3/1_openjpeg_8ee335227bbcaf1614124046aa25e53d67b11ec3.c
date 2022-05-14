 static OPJ_BOOL bmp_read_rle8_data(FILE* IN, OPJ_UINT8* pData,
                                    OPJ_UINT32 stride, OPJ_UINT32 width, OPJ_UINT32 height)
 {
    OPJ_UINT32 x, y;
     OPJ_UINT8 *pix;
     const OPJ_UINT8 *beyond;
 
     beyond = pData + stride * height;
     pix = pData;
 
    x = y = 0U;
     while (y < height) {
         int c = getc(IN);
         if (c == EOF) {
            return OPJ_FALSE;
        }

        if (c) {
            int j, c1_int;
            OPJ_UINT8 c1;

            c1_int = getc(IN);
            if (c1_int == EOF) {
                return OPJ_FALSE;
            }
            c1 = (OPJ_UINT8)c1_int;

             for (j = 0; (j < c) && (x < width) &&
                     ((OPJ_SIZE_T)pix < (OPJ_SIZE_T)beyond); j++, x++, pix++) {
                 *pix = c1;
             }
         } else {
             c = getc(IN);
            if (c == EOF) {
                return OPJ_FALSE;
            }

            if (c == 0x00) { /* EOL */
                x = 0;
                ++y;
                pix = pData + y * stride + x;
            } else if (c == 0x01) { /* EOP */
                break;
            } else if (c == 0x02) { /* MOVE by dxdy */
                c = getc(IN);
                if (c == EOF) {
                    return OPJ_FALSE;
                }
                x += (OPJ_UINT32)c;
                c = getc(IN);
                if (c == EOF) {
                    return OPJ_FALSE;
                }
                y += (OPJ_UINT32)c;
                pix = pData + y * stride + x;
            } else { /* 03 .. 255 */
                int j;
                for (j = 0; (j < c) && (x < width) &&
                        ((OPJ_SIZE_T)pix < (OPJ_SIZE_T)beyond); j++, x++, pix++) {
                    int c1_int;
                    OPJ_UINT8 c1;
                    c1_int = getc(IN);
                    if (c1_int == EOF) {
                        return OPJ_FALSE;
                     }
                     c1 = (OPJ_UINT8)c1_int;
                     *pix = c1;
                 }
                 if ((OPJ_UINT32)c & 1U) { /* skip padding byte */
                     c = getc(IN);
                    if (c == EOF) {
                        return OPJ_FALSE;
                    }
                }
             }
         }
     }/* while() */
     return OPJ_TRUE;
 }
