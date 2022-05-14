local unsigned long crc32_big(crc, buf, len)
    unsigned long crc;
    const unsigned char FAR *buf;
    unsigned len;
{
    register z_crc_t c;
    register const z_crc_t FAR *buf4;

    c = ZSWAP32((z_crc_t)crc);
    c = ~c;
    while (len && ((ptrdiff_t)buf & 3)) {
        c = crc_table[4][(c >> 24) ^ *buf++] ^ (c << 8);
        len--;
     }
 
     buf4 = (const z_crc_t FAR *)(const void FAR *)buf;
     while (len >= 32) {
         DOBIG32;
         len -= 32;
    }
    while (len >= 4) {
         DOBIG4;
         len -= 4;
     }
     buf = (const unsigned char FAR *)buf4;
 
     if (len) do {
        c = crc_table[4][(c >> 24) ^ *buf++] ^ (c << 8);
    } while (--len);
    c = ~c;
    return (unsigned long)(ZSWAP32(c));
}
