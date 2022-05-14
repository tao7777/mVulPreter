     BufferMeta(size_t size, OMX_U32 portIndex)
         : mSize(size),
          mCopyFromOmx(false),
          mCopyToOmx(false),
          mPortIndex(portIndex),
          mBackup(NULL) {
     }
