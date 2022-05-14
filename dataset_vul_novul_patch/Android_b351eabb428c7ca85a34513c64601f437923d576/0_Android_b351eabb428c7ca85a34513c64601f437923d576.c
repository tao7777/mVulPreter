    BufferMeta(const sp<IMemory> &mem, bool is_backup = false)
    BufferMeta(const sp<IMemory> &mem, OMX_U32 portIndex, bool is_backup = false)
         : mMem(mem),
          mIsBackup(is_backup),
          mPortIndex(portIndex) {
     }
