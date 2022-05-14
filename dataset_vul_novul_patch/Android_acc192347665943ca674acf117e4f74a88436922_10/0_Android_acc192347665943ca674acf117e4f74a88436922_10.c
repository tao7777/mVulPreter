status_t FLACParser::init()
{
    mDecoder = FLAC__stream_decoder_new();
 if (mDecoder == NULL) {
        ALOGE("new failed");
 return NO_INIT;
 }
    FLAC__stream_decoder_set_md5_checking(mDecoder, false);
    FLAC__stream_decoder_set_metadata_ignore_all(mDecoder);
    FLAC__stream_decoder_set_metadata_respond(
            mDecoder, FLAC__METADATA_TYPE_STREAMINFO);
    FLAC__stream_decoder_set_metadata_respond(
            mDecoder, FLAC__METADATA_TYPE_PICTURE);
    FLAC__stream_decoder_set_metadata_respond(
            mDecoder, FLAC__METADATA_TYPE_VORBIS_COMMENT);
    FLAC__StreamDecoderInitStatus initStatus;
    initStatus = FLAC__stream_decoder_init_stream(
            mDecoder,
            read_callback, seek_callback, tell_callback,
            length_callback, eof_callback, write_callback,
            metadata_callback, error_callback, (void *) this);
 if (initStatus != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        ALOGE("init_stream failed %d", initStatus);
 return NO_INIT;
 }
 if (!FLAC__stream_decoder_process_until_end_of_metadata(mDecoder)) {
        ALOGE("end_of_metadata failed");
 return NO_INIT;

     }
     if (mStreamInfoValid) {
        if (getChannels() == 0 || getChannels() > kMaxChannels) {
             ALOGE("unsupported channel count %u", getChannels());
             return NO_INIT;
         }
 switch (getBitsPerSample()) {
 case 8:
 case 16:
 case 24:
 break;
 default:
            ALOGE("unsupported bits per sample %u", getBitsPerSample());
 return NO_INIT;
 }
 switch (getSampleRate()) {
 case 8000:
 case 11025:
 case 12000:
 case 16000:
 case 22050:
 case 24000:
 case 32000:
 case 44100:
 case 48000:
 case 88200:
 case 96000:
 break;
 default:
            ALOGE("unsupported sample rate %u", getSampleRate());
 return NO_INIT;
 }

         static const struct {
             unsigned mChannels;
             unsigned mBitsPerSample;
            void (*mCopy)(short *dst, const int * src[kMaxChannels], unsigned nSamples, unsigned nChannels);
         } table[] = {
             { 1,  8, copyMono8    },
             { 2,  8, copyStereo8  },
 { 8, 8, copyMultiCh8  },
 { 1, 16, copyMono16   },
 { 2, 16, copyStereo16 },
 { 8, 16, copyMultiCh16 },
 { 1, 24, copyMono24   },
 { 2, 24, copyStereo24 },
 { 8, 24, copyMultiCh24 },
 };
 for (unsigned i = 0; i < sizeof(table)/sizeof(table[0]); ++i) {
 if (table[i].mChannels >= getChannels() &&
                    table[i].mBitsPerSample == getBitsPerSample()) {
                mCopy = table[i].mCopy;
 break;
 }
 }
 if (mTrackMetadata != 0) {
            mTrackMetadata->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_RAW);
            mTrackMetadata->setInt32(kKeyChannelCount, getChannels());
            mTrackMetadata->setInt32(kKeySampleRate, getSampleRate());
            mTrackMetadata->setInt32(kKeyPcmEncoding, kAudioEncodingPcm16bit);
            mTrackMetadata->setInt64(kKeyDuration,
 (getTotalSamples() * 1000000LL) / getSampleRate());
 }
 } else {
        ALOGE("missing STREAMINFO");
 return NO_INIT;
 }
 if (mFileMetadata != 0) {
        mFileMetadata->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_FLAC);
 }
 return OK;
}
