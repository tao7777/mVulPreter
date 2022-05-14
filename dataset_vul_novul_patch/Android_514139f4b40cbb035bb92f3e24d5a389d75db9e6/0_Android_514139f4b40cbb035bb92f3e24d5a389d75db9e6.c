 static BT_HDR *create_pbuf(UINT16 len, UINT8 *data)
 {
    UINT16 buflen = (UINT16) (len + BTA_HH_MIN_OFFSET + sizeof(BT_HDR));
    if (buflen < len) {
      android_errorWriteWithInfoLog(0x534e4554, "28672558", -1, NULL, 0);
      return NULL;
    }
    BT_HDR* p_buf = GKI_getbuf(buflen);
 
     if (p_buf) {
         UINT8* pbuf_data;

        p_buf->len = len;
        p_buf->offset = BTA_HH_MIN_OFFSET;

        pbuf_data = (UINT8*) (p_buf + 1) + p_buf->offset;
        memcpy(pbuf_data, data, len);
 }
 return p_buf;
}
