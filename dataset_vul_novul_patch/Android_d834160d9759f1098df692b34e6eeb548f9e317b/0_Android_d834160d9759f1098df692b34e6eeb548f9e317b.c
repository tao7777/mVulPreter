OMX_ERRORTYPE SimpleSoftOMXComponent::useBuffer(
        OMX_BUFFERHEADERTYPE **header,
        OMX_U32 portIndex,
        OMX_PTR appPrivate,
        OMX_U32 size,
        OMX_U8 *ptr) {

     Mutex::Autolock autoLock(mLock);
     CHECK_LT(portIndex, mPorts.size());
 
    PortInfo *port = &mPorts.editItemAt(portIndex);
    if (size < port->mDef.nBufferSize) {
        ALOGE("b/63522430, Buffer size is too small.");
        android_errorWriteLog(0x534e4554, "63522430");
        return OMX_ErrorBadParameter;
    }

     *header = new OMX_BUFFERHEADERTYPE;
     (*header)->nSize = sizeof(OMX_BUFFERHEADERTYPE);
     (*header)->nVersion.s.nVersionMajor = 1;
 (*header)->nVersion.s.nVersionMinor = 0;
 (*header)->nVersion.s.nRevision = 0;
 (*header)->nVersion.s.nStep = 0;
 (*header)->pBuffer = ptr;
 (*header)->nAllocLen = size;
 (*header)->nFilledLen = 0;
 (*header)->nOffset = 0;
 (*header)->pAppPrivate = appPrivate;
 (*header)->pPlatformPrivate = NULL;
 (*header)->pInputPortPrivate = NULL;
 (*header)->pOutputPortPrivate = NULL;
 (*header)->hMarkTargetComponent = NULL;
 (*header)->pMarkData = NULL;
 (*header)->nTickCount = 0;
 (*header)->nTimeStamp = 0;
 (*header)->nFlags = 0;

     (*header)->nOutputPortIndex = portIndex;
     (*header)->nInputPortIndex = portIndex;
 
     CHECK(mState == OMX_StateLoaded || port->mDef.bEnabled == OMX_FALSE);
 
     CHECK_LT(port->mBuffers.size(), port->mDef.nBufferCountActual);

    port->mBuffers.push();

 BufferInfo *buffer =
 &port->mBuffers.editItemAt(port->mBuffers.size() - 1);

    buffer->mHeader = *header;
    buffer->mOwnedByUs = false;

 if (port->mBuffers.size() == port->mDef.nBufferCountActual) {
        port->mDef.bPopulated = OMX_TRUE;
        checkTransitions();
 }

 return OMX_ErrorNone;
}
