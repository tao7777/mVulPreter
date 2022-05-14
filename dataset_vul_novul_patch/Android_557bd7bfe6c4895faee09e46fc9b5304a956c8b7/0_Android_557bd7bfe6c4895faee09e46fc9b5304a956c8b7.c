int Visualizer_command(effect_handle_t self, uint32_t cmdCode, uint32_t cmdSize,
 void *pCmdData, uint32_t *replySize, void *pReplyData) {

 VisualizerContext * pContext = (VisualizerContext *)self;
 int retsize;

 if (pContext == NULL || pContext->mState == VISUALIZER_STATE_UNINITIALIZED) {
 return -EINVAL;
 }


 switch (cmdCode) {
 case EFFECT_CMD_INIT:
 if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
 return -EINVAL;
 }
 *(int *) pReplyData = Visualizer_init(pContext);
 break;
 case EFFECT_CMD_SET_CONFIG:
 if (pCmdData == NULL || cmdSize != sizeof(effect_config_t)
 || pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
 return -EINVAL;
 }
 *(int *) pReplyData = Visualizer_setConfig(pContext,
 (effect_config_t *) pCmdData);
 break;
 case EFFECT_CMD_GET_CONFIG:
 if (pReplyData == NULL || replySize == NULL ||
 *replySize != sizeof(effect_config_t)) {
 return -EINVAL;
 }
 Visualizer_getConfig(pContext, (effect_config_t *)pReplyData);
 break;
 case EFFECT_CMD_RESET:
 Visualizer_reset(pContext);
 break;
 case EFFECT_CMD_ENABLE:
 if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
 return -EINVAL;
 }
 if (pContext->mState != VISUALIZER_STATE_INITIALIZED) {
 return -ENOSYS;
 }
        pContext->mState = VISUALIZER_STATE_ACTIVE;
        ALOGV("EFFECT_CMD_ENABLE() OK");
 *(int *)pReplyData = 0;
 break;
 case EFFECT_CMD_DISABLE:
 if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
 return -EINVAL;
 }
 if (pContext->mState != VISUALIZER_STATE_ACTIVE) {
 return -ENOSYS;
 }
        pContext->mState = VISUALIZER_STATE_INITIALIZED;
        ALOGV("EFFECT_CMD_DISABLE() OK");
 *(int *)pReplyData = 0;
 break;
 case EFFECT_CMD_GET_PARAM: {
 if (pCmdData == NULL ||
            cmdSize != (int)(sizeof(effect_param_t) + sizeof(uint32_t)) ||
            pReplyData == NULL || replySize == NULL ||
 *replySize < (int)(sizeof(effect_param_t) + sizeof(uint32_t) + sizeof(uint32_t))) {
 return -EINVAL;
 }
        memcpy(pReplyData, pCmdData, sizeof(effect_param_t) + sizeof(uint32_t));
 effect_param_t *p = (effect_param_t *)pReplyData;
        p->status = 0;
 *replySize = sizeof(effect_param_t) + sizeof(uint32_t);
 if (p->psize != sizeof(uint32_t)) {
            p->status = -EINVAL;
 break;
 }
 switch (*(uint32_t *)p->data) {
 case VISUALIZER_PARAM_CAPTURE_SIZE:
            ALOGV("get mCaptureSize = %" PRIu32, pContext->mCaptureSize);
 *((uint32_t *)p->data + 1) = pContext->mCaptureSize;
            p->vsize = sizeof(uint32_t);
 *replySize += sizeof(uint32_t);
 break;
 case VISUALIZER_PARAM_SCALING_MODE:
            ALOGV("get mScalingMode = %" PRIu32, pContext->mScalingMode);
 *((uint32_t *)p->data + 1) = pContext->mScalingMode;
            p->vsize = sizeof(uint32_t);
 *replySize += sizeof(uint32_t);
 break;
 case VISUALIZER_PARAM_MEASUREMENT_MODE:
            ALOGV("get mMeasurementMode = %" PRIu32, pContext->mMeasurementMode);
 *((uint32_t *)p->data + 1) = pContext->mMeasurementMode;
            p->vsize = sizeof(uint32_t);
 *replySize += sizeof(uint32_t);
 break;
 default:
            p->status = -EINVAL;
 }
 } break;
 case EFFECT_CMD_SET_PARAM: {
 if (pCmdData == NULL ||
            cmdSize != (int)(sizeof(effect_param_t) + sizeof(uint32_t) + sizeof(uint32_t)) ||
            pReplyData == NULL || replySize == NULL || *replySize != sizeof(int32_t)) {
 return -EINVAL;
 }
 *(int32_t *)pReplyData = 0;
 effect_param_t *p = (effect_param_t *)pCmdData;
 if (p->psize != sizeof(uint32_t) || p->vsize != sizeof(uint32_t)) {
 *(int32_t *)pReplyData = -EINVAL;

             break;
         }
         switch (*(uint32_t *)p->data) {
        case VISUALIZER_PARAM_CAPTURE_SIZE: {
            const uint32_t captureSize = *((uint32_t *)p->data + 1);
            if (captureSize > VISUALIZER_CAPTURE_SIZE_MAX) {
                android_errorWriteLog(0x534e4554, "31781965");
                *(int32_t *)pReplyData = -EINVAL;
                ALOGW("set mCaptureSize = %u > %u", captureSize, VISUALIZER_CAPTURE_SIZE_MAX);
            } else {
                pContext->mCaptureSize = captureSize;
                ALOGV("set mCaptureSize = %u", captureSize);
            }
            } break;
         case VISUALIZER_PARAM_SCALING_MODE:
             pContext->mScalingMode = *((uint32_t *)p->data + 1);
             ALOGV("set mScalingMode = %" PRIu32, pContext->mScalingMode);
             break;
        case VISUALIZER_PARAM_LATENCY: {
            uint32_t latency = *((uint32_t *)p->data + 1);
            if (latency > MAX_LATENCY_MS) {
                latency = MAX_LATENCY_MS; // clamp latency b/31781965
            }
            pContext->mLatency = latency;
            ALOGV("set mLatency = %u", latency);
            } break;
         case VISUALIZER_PARAM_MEASUREMENT_MODE:
             pContext->mMeasurementMode = *((uint32_t *)p->data + 1);
             ALOGV("set mMeasurementMode = %" PRIu32, pContext->mMeasurementMode);
 break;
 default:
 *(int32_t *)pReplyData = -EINVAL;
 }
 } break;
 case EFFECT_CMD_SET_DEVICE:
 case EFFECT_CMD_SET_VOLUME:
 case EFFECT_CMD_SET_AUDIO_MODE:
 break;


 case VISUALIZER_CMD_CAPTURE: {
 uint32_t captureSize = pContext->mCaptureSize;
 if (pReplyData == NULL || replySize == NULL || *replySize != captureSize) {
            ALOGV("VISUALIZER_CMD_CAPTURE() error *replySize %" PRIu32 " captureSize %" PRIu32,
 *replySize, captureSize);
 return -EINVAL;
 }
 if (pContext->mState == VISUALIZER_STATE_ACTIVE) {
 const uint32_t deltaMs = Visualizer_getDeltaTimeMsFromUpdatedTime(pContext);

 if ((pContext->mLastCaptureIdx == pContext->mCaptureIdx) &&
 (pContext->mBufferUpdateTime.tv_sec != 0) &&
 (deltaMs > MAX_STALL_TIME_MS)) {
                    ALOGV("capture going to idle");
                    pContext->mBufferUpdateTime.tv_sec = 0;
                    memset(pReplyData, 0x80, captureSize);
 } else {
 int32_t latencyMs = pContext->mLatency;
                latencyMs -= deltaMs;

                 if (latencyMs < 0) {
                     latencyMs = 0;
                 }
                uint32_t deltaSmpl = captureSize
                        + pContext->mConfig.inputCfg.samplingRate * latencyMs / 1000;
 
                // large sample rate, latency, or capture size, could cause overflow.
                // do not offset more than the size of buffer.
                if (deltaSmpl > CAPTURE_BUF_SIZE) {
                    android_errorWriteLog(0x534e4554, "31781965");
                    deltaSmpl = CAPTURE_BUF_SIZE;
                }

                int32_t capturePoint = pContext->mCaptureIdx - deltaSmpl;
                // a negative capturePoint means we wrap the buffer.
                 if (capturePoint < 0) {
                     uint32_t size = -capturePoint;
                     if (size > captureSize) {
                        size = captureSize;
 }
                    memcpy(pReplyData,
                           pContext->mCaptureBuf + CAPTURE_BUF_SIZE + capturePoint,
                           size);
                    pReplyData = (char *)pReplyData + size;
                    captureSize -= size;
                    capturePoint = 0;
 }
                memcpy(pReplyData,
                       pContext->mCaptureBuf + capturePoint,
                       captureSize);
 }

            pContext->mLastCaptureIdx = pContext->mCaptureIdx;
 } else {
            memset(pReplyData, 0x80, captureSize);
 }

 } break;

 case VISUALIZER_CMD_MEASURE: {
 if (pReplyData == NULL || replySize == NULL ||
 *replySize < (sizeof(int32_t) * MEASUREMENT_COUNT)) {
 if (replySize == NULL) {
                ALOGV("VISUALIZER_CMD_MEASURE() error replySize NULL");
 } else {
                ALOGV("VISUALIZER_CMD_MEASURE() error *replySize %" PRIu32
 " < (sizeof(int32_t) * MEASUREMENT_COUNT) %" PRIu32,
 *replySize,
 uint32_t(sizeof(int32_t)) * MEASUREMENT_COUNT);
 }
            android_errorWriteLog(0x534e4554, "30229821");
 return -EINVAL;
 }
 uint16_t peakU16 = 0;
 float sumRmsSquared = 0.0f;
 uint8_t nbValidMeasurements = 0;
 const int32_t delayMs = Visualizer_getDeltaTimeMsFromUpdatedTime(pContext);
 if (delayMs > DISCARD_MEASUREMENTS_TIME_MS) {
            ALOGV("Discarding measurements, last measurement is %" PRId32 "ms old", delayMs);
 for (uint32_t i=0 ; i<pContext->mMeasurementWindowSizeInBuffers ; i++) {
                pContext->mPastMeasurements[i].mIsValid = false;
                pContext->mPastMeasurements[i].mPeakU16 = 0;
                pContext->mPastMeasurements[i].mRmsSquared = 0;
 }
            pContext->mMeasurementBufferIdx = 0;
 } else {
 for (uint32_t i=0 ; i < pContext->mMeasurementWindowSizeInBuffers ; i++) {
 if (pContext->mPastMeasurements[i].mIsValid) {
 if (pContext->mPastMeasurements[i].mPeakU16 > peakU16) {
                        peakU16 = pContext->mPastMeasurements[i].mPeakU16;
 }
                    sumRmsSquared += pContext->mPastMeasurements[i].mRmsSquared;
                    nbValidMeasurements++;
 }
 }
 }
 float rms = nbValidMeasurements == 0 ? 0.0f : sqrtf(sumRmsSquared / nbValidMeasurements);
 int32_t* pIntReplyData = (int32_t*)pReplyData;
 if (rms < 0.000016f) {
            pIntReplyData[MEASUREMENT_IDX_RMS] = -9600; //-96dB
 } else {
            pIntReplyData[MEASUREMENT_IDX_RMS] = (int32_t) (2000 * log10(rms / 32767.0f));
 }
 if (peakU16 == 0) {
            pIntReplyData[MEASUREMENT_IDX_PEAK] = -9600; //-96dB
 } else {
            pIntReplyData[MEASUREMENT_IDX_PEAK] = (int32_t) (2000 * log10(peakU16 / 32767.0f));
 }
        ALOGV("VISUALIZER_CMD_MEASURE peak=%" PRIu16 " (%" PRId32 "mB), rms=%.1f (%" PRId32 "mB)",
                peakU16, pIntReplyData[MEASUREMENT_IDX_PEAK],
                rms, pIntReplyData[MEASUREMENT_IDX_RMS]);
 }
 break;

 default:
        ALOGW("Visualizer_command invalid command %" PRIu32, cmdCode);
 return -EINVAL;
 }

 return 0;
}
