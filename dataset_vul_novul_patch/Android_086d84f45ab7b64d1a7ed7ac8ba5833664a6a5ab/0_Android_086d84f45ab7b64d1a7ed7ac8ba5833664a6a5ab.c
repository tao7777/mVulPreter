status_t OMXNodeInstance::emptyBuffer(
        OMX::buffer_id buffer,
        OMX_U32 rangeOffset, OMX_U32 rangeLength,
        OMX_U32 flags, OMX_TICKS timestamp) {

     Mutex::Autolock autoLock(mLock);
 
     OMX_BUFFERHEADERTYPE *header = findBufferHeader(buffer);
    // rangeLength and rangeOffset must be a subset of the allocated data in the buffer.
    // corner case: we permit rangeOffset == end-of-buffer with rangeLength == 0.
    if (rangeOffset > header->nAllocLen
            || rangeLength > header->nAllocLen - rangeOffset) {
        return BAD_VALUE;
    }
     header->nFilledLen = rangeLength;
     header->nOffset = rangeOffset;
 
 BufferMeta *buffer_meta =
 static_cast<BufferMeta *>(header->pAppPrivate);
    buffer_meta->CopyToOMX(header);

 return emptyBuffer_l(header, flags, timestamp, (intptr_t)buffer);
}
