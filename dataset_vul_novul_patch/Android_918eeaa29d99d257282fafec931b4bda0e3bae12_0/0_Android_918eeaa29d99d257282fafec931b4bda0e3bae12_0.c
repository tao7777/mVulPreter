void SoftAVC::setDecodeArgs(
bool SoftAVC::setDecodeArgs(
         ivd_video_decode_ip_t *ps_dec_ip,
         ivd_video_decode_op_t *ps_dec_op,
         OMX_BUFFERHEADERTYPE *inHeader,
        OMX_BUFFERHEADERTYPE *outHeader,

         size_t timeStampIx) {
     size_t sizeY = outputBufferWidth() * outputBufferHeight();
     size_t sizeUV;
 
     ps_dec_ip->u4_size = sizeof(ivd_video_decode_ip_t);
     ps_dec_op->u4_size = sizeof(ivd_video_decode_op_t);

    ps_dec_ip->e_cmd = IVD_CMD_VIDEO_DECODE;

 /* When in flush and after EOS with zero byte input,
     * inHeader is set to zero. Hence check for non-null */
 if (inHeader) {
        ps_dec_ip->u4_ts = timeStampIx;
        ps_dec_ip->pv_stream_buffer =
            inHeader->pBuffer + inHeader->nOffset;
        ps_dec_ip->u4_num_Bytes = inHeader->nFilledLen;
 } else {
        ps_dec_ip->u4_ts = 0;
        ps_dec_ip->pv_stream_buffer = NULL;

         ps_dec_ip->u4_num_Bytes = 0;
     }
 
     sizeUV = sizeY / 4;
     ps_dec_ip->s_out_buffer.u4_min_out_buf_size[0] = sizeY;
     ps_dec_ip->s_out_buffer.u4_min_out_buf_size[1] = sizeUV;
     ps_dec_ip->s_out_buffer.u4_min_out_buf_size[2] = sizeUV;
 
    uint8_t *pBuf;
    if (outHeader) {
        if (outHeader->nAllocLen < sizeY + (sizeUV * 2)) {
            android_errorWriteLog(0x534e4554, "27569635");
            return false;
        }
        pBuf = outHeader->pBuffer;
    } else {
        // mFlushOutBuffer always has the right size.
        pBuf = mFlushOutBuffer;
    }

     ps_dec_ip->s_out_buffer.pu1_bufs[0] = pBuf;
     ps_dec_ip->s_out_buffer.pu1_bufs[1] = pBuf + sizeY;
     ps_dec_ip->s_out_buffer.pu1_bufs[2] = pBuf + sizeY + sizeUV;
     ps_dec_ip->s_out_buffer.u4_num_bufs = 3;
    return true;
 }
