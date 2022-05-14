OMX::buffer_id OMXNodeInstance::makeBufferID(OMX_BUFFERHEADERTYPE *bufferHeader) {
    return (OMX::buffer_id)bufferHeader;
}
