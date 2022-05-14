 int venc_dev::venc_output_log_buffers(const char *buffer_addr, int buffer_len)
 {
    if (venc_handle->is_secure_session()) {
        DEBUG_PRINT_ERROR("logging secure output buffers is not allowed!");
        return -1;
    }

     if (!m_debug.outfile) {
         int size = 0;
         if(m_sVenc_cfg.codectype == V4L2_PIX_FMT_MPEG4) {
           size = snprintf(m_debug.outfile_name, PROPERTY_VALUE_MAX, "%s/output_enc_%lu_%lu_%p.m4v",
                           m_debug.log_loc, m_sVenc_cfg.input_width, m_sVenc_cfg.input_height, this);
 } else if(m_sVenc_cfg.codectype == V4L2_PIX_FMT_H264) {
           size = snprintf(m_debug.outfile_name, PROPERTY_VALUE_MAX, "%s/output_enc_%lu_%lu_%p.264",
                           m_debug.log_loc, m_sVenc_cfg.input_width, m_sVenc_cfg.input_height, this);
 } else if(m_sVenc_cfg.codectype == V4L2_PIX_FMT_HEVC) {
           size = snprintf(m_debug.outfile_name, PROPERTY_VALUE_MAX, "%s/output_enc_%ld_%ld_%p.265",
                           m_debug.log_loc, m_sVenc_cfg.input_width, m_sVenc_cfg.input_height, this);
 } else if(m_sVenc_cfg.codectype == V4L2_PIX_FMT_H263) {
           size = snprintf(m_debug.outfile_name, PROPERTY_VALUE_MAX, "%s/output_enc_%lu_%lu_%p.263",
                           m_debug.log_loc, m_sVenc_cfg.input_width, m_sVenc_cfg.input_height, this);
 } else if(m_sVenc_cfg.codectype == V4L2_PIX_FMT_VP8) {
           size = snprintf(m_debug.outfile_name, PROPERTY_VALUE_MAX, "%s/output_enc_%lu_%lu_%p.ivf",
                           m_debug.log_loc, m_sVenc_cfg.input_width, m_sVenc_cfg.input_height, this);
 }
 if ((size > PROPERTY_VALUE_MAX) && (size < 0)) {
             DEBUG_PRINT_ERROR("Failed to open output file: %s for logging size:%d",
                                m_debug.outfile_name, size);
 }
        m_debug.outfile = fopen(m_debug.outfile_name, "ab");
 if (!m_debug.outfile) {
            DEBUG_PRINT_ERROR("Failed to open output file: %s for logging errno:%d",
                               m_debug.outfile_name, errno);
            m_debug.outfile_name[0] = '\0';
 return -1;
 }
 }
 if (m_debug.outfile && buffer_len) {
        DEBUG_PRINT_LOW("%s buffer_len:%d", __func__, buffer_len);
        fwrite(buffer_addr, buffer_len, 1, m_debug.outfile);
 }
 return 0;
}
