void SimpleSoftOMXComponent::onPortEnable(OMX_U32 portIndex, bool enable) {
    CHECK_LT(portIndex, mPorts.size());

 PortInfo *port = &mPorts.editItemAt(portIndex);

     CHECK_EQ((int)port->mTransition, (int)PortInfo::NONE);
     CHECK(port->mDef.bEnabled == !enable);
 
    if (port->mDef.eDir != OMX_DirOutput) {
        ALOGE("Port enable/disable allowed only on output ports.");
        notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
        android_errorWriteLog(0x534e4554, "29421804");
        return;
    }

     if (!enable) {
         port->mDef.bEnabled = OMX_FALSE;
         port->mTransition = PortInfo::DISABLING;

 for (size_t i = 0; i < port->mBuffers.size(); ++i) {
 BufferInfo *buffer = &port->mBuffers.editItemAt(i);

 if (buffer->mOwnedByUs) {
                buffer->mOwnedByUs = false;

 if (port->mDef.eDir == OMX_DirInput) {
                    notifyEmptyBufferDone(buffer->mHeader);
 } else {
                    CHECK_EQ(port->mDef.eDir, OMX_DirOutput);
                    notifyFillBufferDone(buffer->mHeader);
 }
 }
 }

        port->mQueue.clear();
 } else {
        port->mTransition = PortInfo::ENABLING;
 }

    checkTransitions();
}
