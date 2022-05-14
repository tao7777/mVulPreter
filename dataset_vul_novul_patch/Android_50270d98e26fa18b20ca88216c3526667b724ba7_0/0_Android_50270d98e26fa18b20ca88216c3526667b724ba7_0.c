status_t SoftVPXEncoder::initEncoder() {
 vpx_codec_err_t codec_return;

    mCodecContext = new vpx_codec_ctx_t;
    mCodecConfiguration = new vpx_codec_enc_cfg_t;
    mCodecInterface = vpx_codec_vp8_cx();

 if (mCodecInterface == NULL) {
 return UNKNOWN_ERROR;
 }
    ALOGD("VP8: initEncoder. BRMode: %u. TSLayers: %zu. KF: %u. QP: %u - %u",
 (uint32_t)mBitrateControlMode, mTemporalLayers, mKeyFrameInterval,
          mMinQuantizer, mMaxQuantizer);
    codec_return = vpx_codec_enc_config_default(mCodecInterface,
                                                mCodecConfiguration,
 0); // Codec specific flags

 if (codec_return != VPX_CODEC_OK) {
        ALOGE("Error populating default configuration for vpx encoder.");
 return UNKNOWN_ERROR;
 }

    mCodecConfiguration->g_w = mWidth;
    mCodecConfiguration->g_h = mHeight;
    mCodecConfiguration->g_threads = GetCPUCoreCount();
    mCodecConfiguration->g_error_resilient = mErrorResilience;

 switch (mLevel) {
 case OMX_VIDEO_VP8Level_Version0:
            mCodecConfiguration->g_profile = 0;
 break;

 case OMX_VIDEO_VP8Level_Version1:
            mCodecConfiguration->g_profile = 1;
 break;

 case OMX_VIDEO_VP8Level_Version2:
            mCodecConfiguration->g_profile = 2;
 break;

 case OMX_VIDEO_VP8Level_Version3:
            mCodecConfiguration->g_profile = 3;
 break;

 default:
            mCodecConfiguration->g_profile = 0;
 }

    mCodecConfiguration->g_timebase.num = 1;
    mCodecConfiguration->g_timebase.den = 1000000;
    mCodecConfiguration->rc_target_bitrate = (mBitrate + 500) / 1000;
    mCodecConfiguration->rc_end_usage = mBitrateControlMode;
    mCodecConfiguration->rc_dropframe_thresh = 0;
 if (mBitrateControlMode == VPX_CBR) {
        mCodecConfiguration->rc_resize_allowed = 0;
        mCodecConfiguration->g_pass = VPX_RC_ONE_PASS;
        mCodecConfiguration->rc_undershoot_pct = 100;
        mCodecConfiguration->rc_overshoot_pct = 15;
        mCodecConfiguration->rc_buf_initial_sz = 500;
        mCodecConfiguration->rc_buf_optimal_sz = 600;
        mCodecConfiguration->rc_buf_sz = 1000;
        mCodecConfiguration->g_error_resilient = 1;
        mCodecConfiguration->g_lag_in_frames = 0;
        mCodecConfiguration->kf_max_dist = 3000;
        mCodecConfiguration->kf_mode = VPX_KF_AUTO;
 }

 switch (mTemporalLayers) {
 case 0:
 {
            mTemporalPatternLength = 0;
 break;
 }
 case 1:
 {
            mCodecConfiguration->ts_number_layers = 1;
            mCodecConfiguration->ts_rate_decimator[0] = 1;
            mCodecConfiguration->ts_periodicity = 1;
            mCodecConfiguration->ts_layer_id[0] = 0;
            mTemporalPattern[0] = kTemporalUpdateLastRefAll;
            mTemporalPatternLength = 1;
 break;
 }
 case 2:
 {
            mCodecConfiguration->ts_number_layers = 2;
            mCodecConfiguration->ts_rate_decimator[0] = 2;
            mCodecConfiguration->ts_rate_decimator[1] = 1;
            mCodecConfiguration->ts_periodicity = 2;
            mCodecConfiguration->ts_layer_id[0] = 0;
            mCodecConfiguration->ts_layer_id[1] = 1;
            mTemporalPattern[0] = kTemporalUpdateLastAndGoldenRefAltRef;
            mTemporalPattern[1] = kTemporalUpdateGoldenWithoutDependencyRefAltRef;
            mTemporalPattern[2] = kTemporalUpdateLastRefAltRef;
            mTemporalPattern[3] = kTemporalUpdateGoldenRefAltRef;
            mTemporalPattern[4] = kTemporalUpdateLastRefAltRef;
            mTemporalPattern[5] = kTemporalUpdateGoldenRefAltRef;
            mTemporalPattern[6] = kTemporalUpdateLastRefAltRef;
            mTemporalPattern[7] = kTemporalUpdateNone;
            mTemporalPatternLength = 8;
 break;
 }
 case 3:
 {
            mCodecConfiguration->ts_number_layers = 3;
            mCodecConfiguration->ts_rate_decimator[0] = 4;
            mCodecConfiguration->ts_rate_decimator[1] = 2;
            mCodecConfiguration->ts_rate_decimator[2] = 1;
            mCodecConfiguration->ts_periodicity = 4;
            mCodecConfiguration->ts_layer_id[0] = 0;
            mCodecConfiguration->ts_layer_id[1] = 2;
            mCodecConfiguration->ts_layer_id[2] = 1;
            mCodecConfiguration->ts_layer_id[3] = 2;
            mTemporalPattern[0] = kTemporalUpdateLastAndGoldenRefAltRef;
            mTemporalPattern[1] = kTemporalUpdateNoneNoRefGoldenRefAltRef;
            mTemporalPattern[2] = kTemporalUpdateGoldenWithoutDependencyRefAltRef;
            mTemporalPattern[3] = kTemporalUpdateNone;
            mTemporalPattern[4] = kTemporalUpdateLastRefAltRef;
            mTemporalPattern[5] = kTemporalUpdateNone;
            mTemporalPattern[6] = kTemporalUpdateGoldenRefAltRef;
            mTemporalPattern[7] = kTemporalUpdateNone;
            mTemporalPatternLength = 8;
 break;
 }
 default:
 {
            ALOGE("Wrong number of temporal layers %zu", mTemporalLayers);
 return UNKNOWN_ERROR;
 }
 }

 for (size_t i = 0; i < mCodecConfiguration->ts_number_layers; i++) {
        mCodecConfiguration->ts_target_bitrate[i] =
            mCodecConfiguration->rc_target_bitrate *
            mTemporalLayerBitrateRatio[i] / 100;
 }
 if (mKeyFrameInterval > 0) {
        mCodecConfiguration->kf_max_dist = mKeyFrameInterval;
        mCodecConfiguration->kf_min_dist = mKeyFrameInterval;
        mCodecConfiguration->kf_mode = VPX_KF_AUTO;
 }
 if (mMinQuantizer > 0) {
        mCodecConfiguration->rc_min_quantizer = mMinQuantizer;
 }
 if (mMaxQuantizer > 0) {
        mCodecConfiguration->rc_max_quantizer = mMaxQuantizer;
 }

    codec_return = vpx_codec_enc_init(mCodecContext,
                                      mCodecInterface,
                                      mCodecConfiguration,
 0); // flags

 if (codec_return != VPX_CODEC_OK) {
        ALOGE("Error initializing vpx encoder");
 return UNKNOWN_ERROR;
 }

    codec_return = vpx_codec_control(mCodecContext,
                                     VP8E_SET_TOKEN_PARTITIONS,
                                     mDCTPartitions);
 if (codec_return != VPX_CODEC_OK) {
        ALOGE("Error setting dct partitions for vpx encoder.");
 return UNKNOWN_ERROR;
 }

 if (mBitrateControlMode == VPX_CBR) {
        codec_return = vpx_codec_control(mCodecContext,
                                         VP8E_SET_STATIC_THRESHOLD,
 1);
 if (codec_return == VPX_CODEC_OK) {
 uint32_t rc_max_intra_target =
                mCodecConfiguration->rc_buf_optimal_sz * (mFramerate >> 17) / 10;
 if (rc_max_intra_target < 300) {
                rc_max_intra_target = 300;
 }
            codec_return = vpx_codec_control(mCodecContext,
                                             VP8E_SET_MAX_INTRA_BITRATE_PCT,
                                             rc_max_intra_target);
 }
 if (codec_return == VPX_CODEC_OK) {
            codec_return = vpx_codec_control(mCodecContext,
                                             VP8E_SET_CPUUSED,
 -8);
 }
 if (codec_return != VPX_CODEC_OK) {
            ALOGE("Error setting cbr parameters for vpx encoder.");
 return UNKNOWN_ERROR;
 }
 }

 
     if (mColorFormat != OMX_COLOR_FormatYUV420Planar || mInputDataIsMeta) {
         free(mConversionBuffer);
        mConversionBuffer = NULL;
        if (((uint64_t)mWidth * mHeight) > ((uint64_t)INT32_MAX / 3)) {
            ALOGE("b/25812794, Buffer size is too big.");
            return UNKNOWN_ERROR;
        }
         mConversionBuffer = (uint8_t *)malloc(mWidth * mHeight * 3 / 2);
         if (mConversionBuffer == NULL) {
             ALOGE("Allocating conversion buffer failed.");
 return UNKNOWN_ERROR;
 }
 }
 return OK;
}
