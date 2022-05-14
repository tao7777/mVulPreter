status_t OMXCodec::configureCodec(const sp<MetaData> &meta) {
    ALOGV("configureCodec protected=%d",
 (mFlags & kEnableGrallocUsageProtected) ? 1 : 0);

 if (!(mFlags & kIgnoreCodecSpecificData)) {
 uint32_t type;
 const void *data;
 size_t size;
 if (meta->findData(kKeyESDS, &type, &data, &size)) {
            ESDS esds((const char *)data, size);
            CHECK_EQ(esds.InitCheck(), (status_t)OK);

 const void *codec_specific_data;
 size_t codec_specific_data_size;
            esds.getCodecSpecificInfo(
 &codec_specific_data, &codec_specific_data_size);

            addCodecSpecificData(
                    codec_specific_data, codec_specific_data_size);
 } else if (meta->findData(kKeyAVCC, &type, &data, &size)) {

 unsigned profile, level;
 status_t err;
 if ((err = parseAVCCodecSpecificData(
                            data, size, &profile, &level)) != OK) {
                ALOGE("Malformed AVC codec specific data.");
 return err;
 }

            CODEC_LOGI(
 "AVC profile = %u (%s), level = %u",
                    profile, AVCProfileToString(profile), level);
 } else if (meta->findData(kKeyHVCC, &type, &data, &size)) {

 unsigned profile, level;
 status_t err;
 if ((err = parseHEVCCodecSpecificData(
                            data, size, &profile, &level)) != OK) {
                ALOGE("Malformed HEVC codec specific data.");
 return err;
 }

            CODEC_LOGI(
 "HEVC profile = %u , level = %u",
                    profile, level);
 } else if (meta->findData(kKeyVorbisInfo, &type, &data, &size)) {
            addCodecSpecificData(data, size);

            CHECK(meta->findData(kKeyVorbisBooks, &type, &data, &size));
            addCodecSpecificData(data, size);
 } else if (meta->findData(kKeyOpusHeader, &type, &data, &size)) {
            addCodecSpecificData(data, size);

            CHECK(meta->findData(kKeyOpusCodecDelay, &type, &data, &size));
            addCodecSpecificData(data, size);
            CHECK(meta->findData(kKeyOpusSeekPreRoll, &type, &data, &size));
            addCodecSpecificData(data, size);
 }
 }

 int32_t bitRate = 0;
 if (mIsEncoder) {
        CHECK(meta->findInt32(kKeyBitRate, &bitRate));
 }
 if (!strcasecmp(MEDIA_MIMETYPE_AUDIO_AMR_NB, mMIME)) {
        setAMRFormat(false /* isWAMR */, bitRate);
 } else if (!strcasecmp(MEDIA_MIMETYPE_AUDIO_AMR_WB, mMIME)) {
        setAMRFormat(true /* isWAMR */, bitRate);
 } else if (!strcasecmp(MEDIA_MIMETYPE_AUDIO_AAC, mMIME)) {
 int32_t numChannels, sampleRate, aacProfile;
        CHECK(meta->findInt32(kKeyChannelCount, &numChannels));
        CHECK(meta->findInt32(kKeySampleRate, &sampleRate));

 if (!meta->findInt32(kKeyAACProfile, &aacProfile)) {
            aacProfile = OMX_AUDIO_AACObjectNull;
 }

 int32_t isADTS;
 if (!meta->findInt32(kKeyIsADTS, &isADTS)) {
            isADTS = false;
 }

 status_t err = setAACFormat(numChannels, sampleRate, bitRate, aacProfile, isADTS);
 if (err != OK) {
            CODEC_LOGE("setAACFormat() failed (err = %d)", err);
 return err;
 }
 } else if (!strcasecmp(MEDIA_MIMETYPE_AUDIO_MPEG, mMIME)) {
 int32_t numChannels, sampleRate;
 if (meta->findInt32(kKeyChannelCount, &numChannels)
 && meta->findInt32(kKeySampleRate, &sampleRate)) {
            setRawAudioFormat(
                    mIsEncoder ? kPortIndexInput : kPortIndexOutput,
                    sampleRate,
                    numChannels);
 }
 } else if (!strcasecmp(MEDIA_MIMETYPE_AUDIO_AC3, mMIME)) {
 int32_t numChannels;
 int32_t sampleRate;
        CHECK(meta->findInt32(kKeyChannelCount, &numChannels));
        CHECK(meta->findInt32(kKeySampleRate, &sampleRate));

 status_t err = setAC3Format(numChannels, sampleRate);
 if (err != OK) {
            CODEC_LOGE("setAC3Format() failed (err = %d)", err);
 return err;
 }
 } else if (!strcasecmp(MEDIA_MIMETYPE_AUDIO_G711_ALAW, mMIME)
 || !strcasecmp(MEDIA_MIMETYPE_AUDIO_G711_MLAW, mMIME)) {

 int32_t sampleRate;
 int32_t numChannels;
        CHECK(meta->findInt32(kKeyChannelCount, &numChannels));
 if (!meta->findInt32(kKeySampleRate, &sampleRate)) {
            sampleRate = 8000;
 }

        setG711Format(sampleRate, numChannels);
 } else if (!strcasecmp(MEDIA_MIMETYPE_AUDIO_RAW, mMIME)) {
        CHECK(!mIsEncoder);

 int32_t numChannels, sampleRate;
        CHECK(meta->findInt32(kKeyChannelCount, &numChannels));
        CHECK(meta->findInt32(kKeySampleRate, &sampleRate));

        setRawAudioFormat(kPortIndexInput, sampleRate, numChannels);
 }

 if (!strncasecmp(mMIME, "video/", 6)) {

 if (mIsEncoder) {
            setVideoInputFormat(mMIME, meta);
 } else {
 status_t err = setVideoOutputFormat(
                    mMIME, meta);

 if (err != OK) {
 return err;
 }
 }
 }

 int32_t maxInputSize;
 if (meta->findInt32(kKeyMaxInputSize, &maxInputSize)) {
        setMinBufferSize(kPortIndexInput, (OMX_U32)maxInputSize);
 }

 
     initOutputFormat(meta);
 
     if (mNativeWindow != NULL
         && !mIsEncoder
         && !strncasecmp(mMIME, "video/", 6)
 && !strncmp(mComponentName, "OMX.", 4)) {
 status_t err = initNativeWindow();
 if (err != OK) {
 return err;
 }
 }

 return OK;
}
