 int venc_dev::venc_input_log_buffers(OMX_BUFFERHEADERTYPE *pbuffer, int fd, int plane_offset) {
    if (venc_handle->is_secure_session()) {
        DEBUG_PRINT_ERROR("logging secure input buffers is not allowed!");
        return -1;
    }

     if (!m_debug.infile) {
         int size = snprintf(m_debug.infile_name, PROPERTY_VALUE_MAX, "%s/input_enc_%lu_%lu_%p.yuv",
                             m_debug.log_loc, m_sVenc_cfg.input_width, m_sVenc_cfg.input_height, this);
 if ((size > PROPERTY_VALUE_MAX) && (size < 0)) {
             DEBUG_PRINT_ERROR("Failed to open output file: %s for logging size:%d",
                                m_debug.infile_name, size);
 }
        m_debug.infile = fopen (m_debug.infile_name, "ab");
 if (!m_debug.infile) {
            DEBUG_PRINT_HIGH("Failed to open input file: %s for logging", m_debug.infile_name);
            m_debug.infile_name[0] = '\0';
 return -1;
 }
 }
 if (m_debug.infile && pbuffer && pbuffer->nFilledLen) {
 unsigned long i, msize;
 int stride = VENUS_Y_STRIDE(COLOR_FMT_NV12, m_sVenc_cfg.input_width);
 int scanlines = VENUS_Y_SCANLINES(COLOR_FMT_NV12, m_sVenc_cfg.input_height);
 unsigned char *pvirt,*ptemp;

 char *temp = (char *)pbuffer->pBuffer;

        msize = VENUS_BUFFER_SIZE(COLOR_FMT_NV12, m_sVenc_cfg.input_width, m_sVenc_cfg.input_height);
 if (metadatamode == 1) {
            pvirt= (unsigned char *)mmap(NULL, msize, PROT_READ|PROT_WRITE,MAP_SHARED, fd, plane_offset);
 if (pvirt) {
               ptemp = pvirt;
 for (i = 0; i < m_sVenc_cfg.input_height; i++) {
                    fwrite(ptemp, m_sVenc_cfg.input_width, 1, m_debug.infile);
                    ptemp += stride;
 }
               ptemp = pvirt + (stride * scanlines);
 for(i = 0; i < m_sVenc_cfg.input_height/2; i++) {
                   fwrite(ptemp, m_sVenc_cfg.input_width, 1, m_debug.infile);
                   ptemp += stride;
 }
               munmap(pvirt, msize);
 } else if (pvirt == MAP_FAILED) {
                 DEBUG_PRINT_ERROR("%s mmap failed", __func__);
 return -1;
 }
 } else {
 for (i = 0; i < m_sVenc_cfg.input_height; i++) {
                 fwrite(temp, m_sVenc_cfg.input_width, 1, m_debug.infile);
                 temp += stride;
 }

            temp = (char *)pbuffer->pBuffer + (stride * scanlines);

 for(i = 0; i < m_sVenc_cfg.input_height/2; i++) {
                fwrite(temp, m_sVenc_cfg.input_width, 1, m_debug.infile);
                temp += stride;
 }
 }
 }
 return 0;
}
