static vpx_codec_err_t vp8_peek_si_internal(const uint8_t *data,
 unsigned int data_sz,
 vpx_codec_stream_info_t *si,
                                            vpx_decrypt_cb decrypt_cb,
 void *decrypt_state)
{
 vpx_codec_err_t res = VPX_CODEC_OK;

 if(data + data_sz <= data)
 {
        res = VPX_CODEC_INVALID_PARAM;
 }
 else
 {
 /* Parse uncompresssed part of key frame header.
         * 3 bytes:- including version, frame type and an offset
         * 3 bytes:- sync code (0x9d, 0x01, 0x2a)
         * 4 bytes:- including image width and height in the lowest 14 bits
         *           of each 2-byte value.
         */
 uint8_t clear_buffer[10];
 const uint8_t *clear = data;
 if (decrypt_cb)
 {
 int n = MIN(sizeof(clear_buffer), data_sz);
            decrypt_cb(decrypt_state, data, clear_buffer, n);
            clear = clear_buffer;
 }
        si->is_kf = 0;

 if (data_sz >= 10 && !(clear[0] & 0x01)) /* I-Frame */
 {
            si->is_kf = 1;

 /* vet via sync code */
 if (clear[3] != 0x9d || clear[4] != 0x01 || clear[5] != 0x2a)
 return VPX_CODEC_UNSUP_BITSTREAM;

            si->w = (clear[6] | (clear[7] << 8)) & 0x3fff;

             si->h = (clear[8] | (clear[9] << 8)) & 0x3fff;
 
             /*printf("w=%d, h=%d\n", si->w, si->h);*/
            if (!(si->h && si->w))
                res = VPX_CODEC_CORRUPT_FRAME;
         }
         else
         {
            res = VPX_CODEC_UNSUP_BITSTREAM;
 }
 }

 return res;
}
