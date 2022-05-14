OMX_ERRORTYPE  omx_vdec::component_deinit(OMX_IN OMX_HANDLETYPE hComp)
{
 (void) hComp;
#ifdef _ANDROID_
 if (iDivXDrmDecrypt) {
 delete iDivXDrmDecrypt;
        iDivXDrmDecrypt=NULL;
 }
#endif //_ANDROID_

 unsigned i = 0;
 if (OMX_StateLoaded != m_state) {
        DEBUG_PRINT_ERROR("WARNING:Rxd DeInit,OMX not in LOADED state %d",\
                m_state);
        DEBUG_PRINT_ERROR("Playback Ended - FAILED");
 } else {
        DEBUG_PRINT_HIGH("Playback Ended - PASSED");
 }

 /*Check if the output buffers have to be cleaned up*/

     if (m_out_mem_ptr) {
         DEBUG_PRINT_LOW("Freeing the Output Memory");
         for (i = 0; i < drv_ctx.op_buf.actualcount; i++ ) {
            free_output_buffer (&m_out_mem_ptr[i]);
         }
 #ifdef _ANDROID_ICS_
         memset(&native_buffer, 0, (sizeof(nativebuffer) * MAX_NUM_INPUT_OUTPUT_BUFFERS));
#endif
 }

 /*Check if the input buffers have to be cleaned up*/

     if (m_inp_mem_ptr || m_inp_heap_ptr) {
         DEBUG_PRINT_LOW("Freeing the Input Memory");
         for (i = 0; i<drv_ctx.ip_buf.actualcount; i++ ) {
            if (m_inp_mem_ptr)
                free_input_buffer (i,&m_inp_mem_ptr[i]);
            else
                free_input_buffer (i,NULL);
        }
     }
     free_input_buffer_header();
     free_output_buffer_header();
 if (h264_scratch.pBuffer) {
        free(h264_scratch.pBuffer);
        h264_scratch.pBuffer = NULL;
 }

 if (h264_parser) {
 delete h264_parser;
        h264_parser = NULL;
 }

 if (m_frame_parser.mutils) {
        DEBUG_PRINT_LOW("Free utils parser");
 delete (m_frame_parser.mutils);
        m_frame_parser.mutils = NULL;
 }

 if (m_platform_list) {
        free(m_platform_list);
        m_platform_list = NULL;
 }
 if (m_vendor_config.pData) {
        free(m_vendor_config.pData);
        m_vendor_config.pData = NULL;
 }

    m_ftb_q.m_size=0;
    m_cmd_q.m_size=0;
    m_etb_q.m_size=0;
    m_ftb_q.m_read = m_ftb_q.m_write =0;
    m_cmd_q.m_read = m_cmd_q.m_write =0;
    m_etb_q.m_read = m_etb_q.m_write =0;
#ifdef _ANDROID_
 if (m_debug_timestamp) {
        m_timestamp_list.reset_ts_list();
 }
#endif

    DEBUG_PRINT_LOW("Calling VDEC_IOCTL_STOP_NEXT_MSG");
    DEBUG_PRINT_HIGH("Close the driver instance");

 if (m_debug.infile) {
        fclose(m_debug.infile);
        m_debug.infile = NULL;
 }
 if (m_debug.outfile) {
        fclose(m_debug.outfile);
        m_debug.outfile = NULL;
 }
#ifdef OUTPUT_EXTRADATA_LOG
 if (outputExtradataFile)
        fclose (outputExtradataFile);
#endif
    DEBUG_PRINT_INFO("omx_vdec::component_deinit() complete");
 return OMX_ErrorNone;
}
