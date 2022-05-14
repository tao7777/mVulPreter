    BufferMeta(const sp<IMemory> &mem, OMX_U32 portIndex, bool is_backup = false)
    BufferMeta(
            const sp<IMemory> &mem, OMX_U32 portIndex, bool copyToOmx,
            bool copyFromOmx, OMX_U8 *backup)
         : mMem(mem),
          mCopyFromOmx(copyFromOmx),
          mCopyToOmx(copyToOmx),
          mPortIndex(portIndex),
          mBackup(backup) {
     }
