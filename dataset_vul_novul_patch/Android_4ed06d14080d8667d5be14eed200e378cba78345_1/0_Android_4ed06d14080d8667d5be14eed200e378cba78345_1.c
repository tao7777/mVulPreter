OMX_ERRORTYPE  omx_video::fill_this_buffer(OMX_IN OMX_HANDLETYPE  hComp,

         OMX_IN OMX_BUFFERHEADERTYPE* buffer)
 {
     DEBUG_PRINT_LOW("FTB: buffer->pBuffer[%p]", buffer->pBuffer);
    if (m_state != OMX_StateExecuting &&
            m_state != OMX_StatePause &&
            m_state != OMX_StateIdle) {
         DEBUG_PRINT_ERROR("ERROR: FTB in Invalid State");
         return OMX_ErrorInvalidState;
     }

 if (buffer == NULL ||(buffer->nSize != sizeof(OMX_BUFFERHEADERTYPE))) {
        DEBUG_PRINT_ERROR("ERROR: omx_video::ftb-->Invalid buffer or size");
 return OMX_ErrorBadParameter;
 }

 if (buffer->nVersion.nVersion != OMX_SPEC_VERSION) {
        DEBUG_PRINT_ERROR("ERROR: omx_video::ftb-->OMX Version Invalid");
 return OMX_ErrorVersionMismatch;
 }

 if (buffer->nOutputPortIndex != (OMX_U32)PORT_INDEX_OUT) {
        DEBUG_PRINT_ERROR("ERROR: omx_video::ftb-->Bad port index");
 return OMX_ErrorBadPortIndex;
 }

 if (!m_sOutPortDef.bEnabled) {
        DEBUG_PRINT_ERROR("ERROR: omx_video::ftb-->port is disabled");
 return OMX_ErrorIncorrectStateOperation;
 }

    post_event((unsigned long) hComp, (unsigned long)buffer,OMX_COMPONENT_GENERATE_FTB);
 return OMX_ErrorNone;
}
