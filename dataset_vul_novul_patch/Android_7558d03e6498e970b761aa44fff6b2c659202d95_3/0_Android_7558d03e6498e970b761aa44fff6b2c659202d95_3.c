int omx_venc::async_message_process (void *context, void* message)
{
    omx_video* omx = NULL;
 struct venc_msg *m_sVenc_msg = NULL;
    OMX_BUFFERHEADERTYPE* omxhdr = NULL;
 struct venc_buffer *temp_buff = NULL;

 if (context == NULL || message == NULL) {
        DEBUG_PRINT_ERROR("ERROR: omx_venc::async_message_process invalid i/p params");
 return -1;
 }
    m_sVenc_msg = (struct venc_msg *)message;

    omx = reinterpret_cast<omx_video*>(context);

 if (m_sVenc_msg->statuscode != VEN_S_SUCCESS) {
        DEBUG_PRINT_ERROR("ERROR: async_msg_process() - Error statuscode = %lu",
                m_sVenc_msg->statuscode);
 if(m_sVenc_msg->msgcode == VEN_MSG_HW_OVERLOAD) {
            omx->omx_report_hw_overload();
 } else
        omx->omx_report_error();
 }

    DEBUG_PRINT_LOW("omx_venc::async_message_process- msgcode = %lu",
            m_sVenc_msg->msgcode);
 switch (m_sVenc_msg->msgcode) {
 case VEN_MSG_START:
            omx->post_event (0,m_sVenc_msg->statuscode,\
                    OMX_COMPONENT_GENERATE_START_DONE);
 break;
 case VEN_MSG_STOP:
            omx->post_event (0,m_sVenc_msg->statuscode,\
                    OMX_COMPONENT_GENERATE_STOP_DONE);
 break;
 case VEN_MSG_RESUME:
            omx->post_event (0,m_sVenc_msg->statuscode,\
                    OMX_COMPONENT_GENERATE_RESUME_DONE);
 break;
 case VEN_MSG_PAUSE:
            omx->post_event (0,m_sVenc_msg->statuscode,\
                    OMX_COMPONENT_GENERATE_PAUSE_DONE);
 break;
 case VEN_MSG_FLUSH_INPUT_DONE:

            omx->post_event (0,m_sVenc_msg->statuscode,\
                    OMX_COMPONENT_GENERATE_EVENT_INPUT_FLUSH);
 break;
 case VEN_MSG_FLUSH_OUPUT_DONE:
            omx->post_event (0,m_sVenc_msg->statuscode,\
                    OMX_COMPONENT_GENERATE_EVENT_OUTPUT_FLUSH);
 break;
 case VEN_MSG_INPUT_BUFFER_DONE:
            omxhdr = (OMX_BUFFERHEADERTYPE* )\
                     m_sVenc_msg->buf.clientdata;

 if (omxhdr == NULL ||
 (((OMX_U32)(omxhdr - omx->m_inp_mem_ptr) > omx->m_sInPortDef.nBufferCountActual) &&
 ((OMX_U32)(omxhdr - omx->meta_buffer_hdr) > omx->m_sInPortDef.nBufferCountActual))) {
                omxhdr = NULL;
                m_sVenc_msg->statuscode = VEN_S_EFAIL;
 }

#ifdef _ANDROID_ICS_
            omx->omx_release_meta_buffer(omxhdr);
#endif
            omx->post_event ((unsigned long)omxhdr,m_sVenc_msg->statuscode,
                    OMX_COMPONENT_GENERATE_EBD);
 break;
 case VEN_MSG_OUTPUT_BUFFER_DONE:
            omxhdr = (OMX_BUFFERHEADERTYPE*)m_sVenc_msg->buf.clientdata;

 if ( (omxhdr != NULL) &&
 ((OMX_U32)(omxhdr - omx->m_out_mem_ptr) < omx->m_sOutPortDef.nBufferCountActual)) {
 if (m_sVenc_msg->buf.len <=  omxhdr->nAllocLen) {
                    omxhdr->nFilledLen = m_sVenc_msg->buf.len;
                    omxhdr->nOffset = m_sVenc_msg->buf.offset;
                    omxhdr->nTimeStamp = m_sVenc_msg->buf.timestamp;
                    DEBUG_PRINT_LOW("o/p TS = %u", (unsigned int)m_sVenc_msg->buf.timestamp);

                     omxhdr->nFlags = m_sVenc_msg->buf.flags;
 
                     /*Use buffer case*/
                    if (omx->output_use_buffer && !omx->m_use_output_pmem && !omx->is_secure_session()) {
                         DEBUG_PRINT_LOW("memcpy() for o/p Heap UseBuffer");
                         memcpy(omxhdr->pBuffer,
                                 (m_sVenc_msg->buf.ptrbuffer),
                                m_sVenc_msg->buf.len);
 }
 } else {
                    omxhdr->nFilledLen = 0;
 }

 } else {
                omxhdr = NULL;
                m_sVenc_msg->statuscode = VEN_S_EFAIL;
 }
            omx->post_event ((unsigned long)omxhdr,m_sVenc_msg->statuscode,
                    OMX_COMPONENT_GENERATE_FBD);
 break;
 case VEN_MSG_NEED_OUTPUT_BUFFER:
 break;
#ifndef _MSM8974_
 case VEN_MSG_LTRUSE_FAILED:
            DEBUG_PRINT_ERROR("LTRUSE Failed!");
            omx->post_event (NULL,m_sVenc_msg->statuscode,
                    OMX_COMPONENT_GENERATE_LTRUSE_FAILED);
 break;
#endif
 default:
            DEBUG_PRINT_HIGH("Unknown msg received : %lu", m_sVenc_msg->msgcode);
 break;
 }
 return 0;
}
