int ff_amf_get_field_value(const uint8_t *data, const uint8_t *data_end,
static int amf_get_field_value2(GetByteContext *gb,
                            const uint8_t *name, uint8_t *dst, int dst_size)
 {
     int namelen = strlen(name);
     int len;
 
    while (bytestream2_peek_byte(gb) != AMF_DATA_TYPE_OBJECT && bytestream2_get_bytes_left(gb) > 0) {
        int ret = amf_tag_skip(gb);
        if (ret < 0)
            return -1;
     }
    if (bytestream2_get_bytes_left(gb) < 3)
         return -1;
    bytestream2_get_byte(gb);

     for (;;) {
        int size = bytestream2_get_be16(gb);
         if (!size)
             break;
        if (size < 0 || size >= bytestream2_get_bytes_left(gb))
             return -1;
        bytestream2_skip(gb, size);
        if (size == namelen && !memcmp(gb->buffer-size, name, namelen)) {
            switch (bytestream2_get_byte(gb)) {
             case AMF_DATA_TYPE_NUMBER:
                snprintf(dst, dst_size, "%g", av_int2double(bytestream2_get_be64(gb)));
                 break;
             case AMF_DATA_TYPE_BOOL:
                snprintf(dst, dst_size, "%s", bytestream2_get_byte(gb) ? "true" : "false");
                 break;
             case AMF_DATA_TYPE_STRING:
                len = bytestream2_get_be16(gb);
                if (dst_size < 1)
                    return -1;
                if (dst_size < len + 1)
                    len = dst_size - 1;
                bytestream2_get_buffer(gb, dst, len);
                dst[len] = 0;
                 break;
             default:
                 return -1;
             }
             return 0;
         }
        len = amf_tag_skip(gb);
        if (len < 0 || bytestream2_get_bytes_left(gb) <= 0)
             return -1;
     }
     return -1;
 }
