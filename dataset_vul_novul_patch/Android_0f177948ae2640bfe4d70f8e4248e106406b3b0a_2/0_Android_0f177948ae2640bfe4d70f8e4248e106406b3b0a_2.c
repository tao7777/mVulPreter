     void CopyFromOMX(const OMX_BUFFERHEADERTYPE *header) {
        if (!mCopyFromOmx) {
             return;
         }
 
        sp<ABuffer> codec = getBuffer(header, false /* backup */, true /* limit */);

        memcpy((OMX_U8 *)mMem->pointer() + header->nOffset, codec->data(), codec->size());

     }
