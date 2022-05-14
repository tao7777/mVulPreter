status_t OMXNodeInstance::updateGraphicBufferInMeta(

         OMX_U32 portIndex, const sp<GraphicBuffer>& graphicBuffer,
         OMX::buffer_id buffer) {
     Mutex::Autolock autoLock(mLock);
    OMX_BUFFERHEADERTYPE *header = findBufferHeader(buffer);
     return updateGraphicBufferInMeta_l(portIndex, graphicBuffer, buffer, header);
 }
