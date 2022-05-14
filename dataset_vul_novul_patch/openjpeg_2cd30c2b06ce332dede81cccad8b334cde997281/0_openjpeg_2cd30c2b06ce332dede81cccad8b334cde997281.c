static unsigned short get_ushort(const unsigned char *data)
/* Returns a ushort from a little-endian serialized value */
static unsigned short get_tga_ushort(const unsigned char *data)
 {
    return data[0] | (data[1] << 8);
 }
