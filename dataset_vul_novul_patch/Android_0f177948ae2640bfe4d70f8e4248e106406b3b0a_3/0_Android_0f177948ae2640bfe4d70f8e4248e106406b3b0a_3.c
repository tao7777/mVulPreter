     void CopyToOMX(const OMX_BUFFERHEADERTYPE *header) {
        if (!mCopyToOmx) {
             return;
         }
 
        memcpy(header->pBuffer + header->nOffset,
 (const OMX_U8 *)mMem->pointer() + header->nOffset,
                header->nFilledLen);
 }
