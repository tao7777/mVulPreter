OMX_ERRORTYPE SimpleSoftOMXComponent::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamPortDefinition:
 {

             OMX_PARAM_PORTDEFINITIONTYPE *defParams =
                 (OMX_PARAM_PORTDEFINITIONTYPE *)params;
 
             if (defParams->nPortIndex >= mPorts.size()
                     || defParams->nSize
                             != sizeof(OMX_PARAM_PORTDEFINITIONTYPE)) {
 return OMX_ErrorUndefined;
 }

 const PortInfo *port =
 &mPorts.itemAt(defParams->nPortIndex);

            memcpy(defParams, &port->mDef, sizeof(port->mDef));

 return OMX_ErrorNone;
 }

 default:
 return OMX_ErrorUnsupportedIndex;
 }
}
