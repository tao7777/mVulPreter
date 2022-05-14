SupportsType MimeUtil::IsCodecSupported(const std::string& mime_type_lower_case,
                                         Codec codec,
                                         VideoCodecProfile video_profile,
                                         uint8_t video_level,
                                        gfx::ColorSpace::TransferID eotf,
                                         bool is_encrypted) const {
   DCHECK_EQ(base::ToLowerASCII(mime_type_lower_case), mime_type_lower_case);
   DCHECK_NE(codec, INVALID_CODEC);

  VideoCodec video_codec = MimeUtilToVideoCodec(codec);
  if (video_codec != kUnknownVideoCodec &&
      video_codec != kCodecTheora && video_codec != kCodecVP8) {
    DCHECK_NE(video_profile, VIDEO_CODEC_PROFILE_UNKNOWN);
    DCHECK_GT(video_level, 0);
  }

  if (!allow_proprietary_codecs_ && IsCodecProprietary(codec)) {
    return IsNotSupported;
  }

  bool ambiguous_platform_support = false;
  if (codec == MimeUtil::H264) {
    switch (video_profile) {
      case H264PROFILE_BASELINE:
      case H264PROFILE_MAIN:
      case H264PROFILE_HIGH:
        break;
#if !defined(MEDIA_DISABLE_FFMPEG) && !defined(OS_ANDROID)
      case H264PROFILE_HIGH10PROFILE:
        ambiguous_platform_support = is_encrypted;
        break;
#endif
      default:
        ambiguous_platform_support = true;
    }
  } else if (codec == MimeUtil::VP9 && video_profile != VP9PROFILE_PROFILE0) {
    ambiguous_platform_support = true;
  }
 
   if (GetMediaClient() && video_codec != kUnknownVideoCodec &&
       !GetMediaClient()->IsSupportedVideoConfig(
          {video_codec, video_profile, video_level, eotf})) {
     return IsNotSupported;
   }
 
#if defined(OS_ANDROID)
  if (!IsCodecSupportedOnAndroid(codec, mime_type_lower_case, is_encrypted,
                                 platform_info_)) {
    return IsNotSupported;
  }
#endif

  return ambiguous_platform_support ? MayBeSupported : IsSupported;
}
