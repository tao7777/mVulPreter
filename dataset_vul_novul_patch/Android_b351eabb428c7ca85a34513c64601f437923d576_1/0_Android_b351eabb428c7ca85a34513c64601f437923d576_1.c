    BufferMeta(const sp<GraphicBuffer> &graphicBuffer)
    BufferMeta(const sp<GraphicBuffer> &graphicBuffer, OMX_U32 portIndex)
         : mGraphicBuffer(graphicBuffer),
          mIsBackup(false),
          mPortIndex(portIndex) {
     }
