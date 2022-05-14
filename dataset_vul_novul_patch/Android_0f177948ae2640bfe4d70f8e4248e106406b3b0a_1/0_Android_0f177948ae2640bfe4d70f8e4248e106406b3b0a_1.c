     BufferMeta(const sp<GraphicBuffer> &graphicBuffer, OMX_U32 portIndex)
         : mGraphicBuffer(graphicBuffer),
          mCopyFromOmx(false),
          mCopyToOmx(false),
          mPortIndex(portIndex),
          mBackup(NULL) {
     }
