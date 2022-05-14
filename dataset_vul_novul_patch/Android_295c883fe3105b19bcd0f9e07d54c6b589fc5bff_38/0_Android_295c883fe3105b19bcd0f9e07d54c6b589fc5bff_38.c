OMX_ERRORTYPE SoftVideoDecoderOMXComponent::getConfig(
        OMX_INDEXTYPE index, OMX_PTR params) {
 switch (index) {
 case OMX_IndexConfigCommonOutputCrop:

         {
             OMX_CONFIG_RECTTYPE *rectParams = (OMX_CONFIG_RECTTYPE *)params;
 
            if (!isValidOMXParam(rectParams)) {
                return OMX_ErrorBadParameter;
            }

             if (rectParams->nPortIndex != kOutputPortIndex) {
                 return OMX_ErrorUndefined;
             }

            rectParams->nLeft = mCropLeft;
            rectParams->nTop = mCropTop;
            rectParams->nWidth = mCropWidth;
            rectParams->nHeight = mCropHeight;

 return OMX_ErrorNone;
 }

 default:
 return OMX_ErrorUnsupportedIndex;
 }
}
