void OMXNodeInstance::onEvent(
        OMX_EVENTTYPE event, OMX_U32 arg1, OMX_U32 arg2) {
 const char *arg1String = "??";
 const char *arg2String = "??";
 ADebug::Level level = ADebug::kDebugInternalState;

 switch (event) {
 case OMX_EventCmdComplete:
            arg1String = asString((OMX_COMMANDTYPE)arg1);
 switch (arg1) {
 case OMX_CommandStateSet:
                    arg2String = asString((OMX_STATETYPE)arg2);
                    level = ADebug::kDebugState;
 break;
 case OMX_CommandFlush:
 case OMX_CommandPortEnable:
 {
 Mutex::Autolock _l(mDebugLock);
                    bumpDebugLevel_l(2 /* numInputBuffers */, 2 /* numOutputBuffers */);
 }
 default:
                    arg2String = portString(arg2);
 }
 break;
 case OMX_EventError:
            arg1String = asString((OMX_ERRORTYPE)arg1);
            level = ADebug::kDebugLifeCycle;
 break;
 case OMX_EventPortSettingsChanged:
            arg2String = asString((OMX_INDEXEXTTYPE)arg2);
 default:
            arg1String = portString(arg1);
 }

    CLOGI_(level, onEvent, "%s(%x), %s(%x), %s(%x)",
            asString(event), event, arg1String, arg1, arg2String, arg2);
 const sp<GraphicBufferSource>& bufferSource(getGraphicBufferSource());

 if (bufferSource != NULL
 && event == OMX_EventCmdComplete
 && arg1 == OMX_CommandStateSet

             && arg2 == OMX_StateExecuting) {
         bufferSource->omxExecuting();
     }

    // allow configuration if we return to the loaded state
    if (event == OMX_EventCmdComplete
            && arg1 == OMX_CommandStateSet
            && arg2 == OMX_StateLoaded) {
        mSailed = false;
    }
 }
