    BufferMeta(size_t size)
    BufferMeta(size_t size, OMX_U32 portIndex)
         : mSize(size),
          mIsBackup(false),
          mPortIndex(portIndex) {
     }
