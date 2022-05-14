OMX_ERRORTYPE SimpleSoftOMXComponent::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamPortDefinition:
 {

             OMX_PARAM_PORTDEFINITIONTYPE *defParams =
                 (OMX_PARAM_PORTDEFINITIONTYPE *)params;
 
             if (defParams->nPortIndex >= mPorts.size()) {
                 return OMX_ErrorBadPortIndex;
             }
 if (defParams->nSize != sizeof(OMX_PARAM_PORTDEFINITIONTYPE)) {
 return OMX_ErrorUnsupportedSetting;
 }

 PortInfo *port =
 &mPorts.editItemAt(defParams->nPortIndex);

 if (defParams->nBufferSize > port->mDef.nBufferSize) {
                port->mDef.nBufferSize = defParams->nBufferSize;
 }

 if (defParams->nBufferCountActual < port->mDef.nBufferCountMin) {
                ALOGW("component requires at least %u buffers (%u requested)",
                        port->mDef.nBufferCountMin, defParams->nBufferCountActual);
 return OMX_ErrorUnsupportedSetting;
 }

            port->mDef.nBufferCountActual = defParams->nBufferCountActual;
 return OMX_ErrorNone;
 }

 default:
 return OMX_ErrorUnsupportedIndex;
 }
}
