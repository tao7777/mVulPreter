status_t NuPlayer::GenericSource::initFromDataSource() {
    sp<MediaExtractor> extractor;
 String8 mimeType;
 float confidence;
    sp<AMessage> dummy;
 bool isWidevineStreaming = false;

    CHECK(mDataSource != NULL);

 if (mIsWidevine) {
        isWidevineStreaming = SniffWVM(
                mDataSource, &mimeType, &confidence, &dummy);
 if (!isWidevineStreaming ||
                strcasecmp(
                    mimeType.string(), MEDIA_MIMETYPE_CONTAINER_WVM)) {
            ALOGE("unsupported widevine mime: %s", mimeType.string());
 return UNKNOWN_ERROR;
 }
 } else if (mIsStreaming) {
 if (!mDataSource->sniff(&mimeType, &confidence, &dummy)) {
 return UNKNOWN_ERROR;
 }
        isWidevineStreaming = !strcasecmp(
                mimeType.string(), MEDIA_MIMETYPE_CONTAINER_WVM);
 }

 if (isWidevineStreaming) {
        mCachedSource.clear();
        mDataSource = mHttpSource;
        mWVMExtractor = new WVMExtractor(mDataSource);
        mWVMExtractor->setAdaptiveStreamingMode(true);
 if (mUIDValid) {
            mWVMExtractor->setUID(mUID);
 }
        extractor = mWVMExtractor;
 } else {
        extractor = MediaExtractor::Create(mDataSource,
                mimeType.isEmpty() ? NULL : mimeType.string());
 }

 if (extractor == NULL) {
 return UNKNOWN_ERROR;
 }

 if (extractor->getDrmFlag()) {
        checkDrmStatus(mDataSource);
 }

    mFileMeta = extractor->getMetaData();
 if (mFileMeta != NULL) {
 int64_t duration;
 if (mFileMeta->findInt64(kKeyDuration, &duration)) {
            mDurationUs = duration;
 }

 if (!mIsWidevine) {
 const char *fileMime;
 if (mFileMeta->findCString(kKeyMIMEType, &fileMime)
 && !strncasecmp(fileMime, "video/wvm", 9)) {
                mIsWidevine = true;
 }
 }
 }

 int32_t totalBitrate = 0;

 size_t numtracks = extractor->countTracks();
 if (numtracks == 0) {
 return UNKNOWN_ERROR;
 }

 
     for (size_t i = 0; i < numtracks; ++i) {
         sp<MediaSource> track = extractor->getTrack(i);
        if (track == NULL) {
            continue;
        }
 
         sp<MetaData> meta = extractor->getTrackMetaData(i);
 
 const char *mime;
        CHECK(meta->findCString(kKeyMIMEType, &mime));

 if (!strncasecmp(mime, "audio/", 6)) {
 if (mAudioTrack.mSource == NULL) {
                mAudioTrack.mIndex = i;
                mAudioTrack.mSource = track;
                mAudioTrack.mPackets =
 new AnotherPacketSource(mAudioTrack.mSource->getFormat());

 if (!strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_VORBIS)) {
                    mAudioIsVorbis = true;
 } else {
                    mAudioIsVorbis = false;
 }
 }
 } else if (!strncasecmp(mime, "video/", 6)) {
 if (mVideoTrack.mSource == NULL) {
                mVideoTrack.mIndex = i;
                mVideoTrack.mSource = track;
                mVideoTrack.mPackets =
 new AnotherPacketSource(mVideoTrack.mSource->getFormat());

 int32_t secure;
 if (meta->findInt32(kKeyRequiresSecureBuffers, &secure)
 && secure) {
                    mIsSecure = true;
 if (mUIDValid) {
                        extractor->setUID(mUID);
 }
 }

             }
         }
 
        mSources.push(track);
        int64_t durationUs;
        if (meta->findInt64(kKeyDuration, &durationUs)) {
            if (durationUs > mDurationUs) {
                mDurationUs = durationUs;
             }
         }

        int32_t bitrate;
        if (totalBitrate >= 0 && meta->findInt32(kKeyBitRate, &bitrate)) {
            totalBitrate += bitrate;
        } else {
            totalBitrate = -1;
        }
    }

    if (mSources.size() == 0) {
        ALOGE("b/23705695");
        return UNKNOWN_ERROR;
     }
 
     mBitrate = totalBitrate;

 return OK;
}
