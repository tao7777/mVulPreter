SupportsType MimeUtil::AreSupportedCodecs(
    const CodecSet& supported_codecs,
    const std::vector<std::string>& codecs,
    const std::string& mime_type_lower_case,
    bool is_encrypted) const {
  DCHECK(!supported_codecs.empty());
  DCHECK(!codecs.empty());
  DCHECK_EQ(base::ToLowerASCII(mime_type_lower_case), mime_type_lower_case);

  SupportsType combined_result = IsSupported;

  for (size_t i = 0; i < codecs.size(); ++i) {
    bool ambiguous_codec_string = false;
     Codec codec = INVALID_CODEC;
     VideoCodecProfile video_profile = VIDEO_CODEC_PROFILE_UNKNOWN;
     uint8_t video_level = 0;
    VideoColorSpace color_space;
     if (!ParseCodecString(mime_type_lower_case, codecs[i], &codec,
                           &ambiguous_codec_string, &video_profile, &video_level,
                          &color_space)) {
       return IsNotSupported;
     }
 
    if (supported_codecs.find(codec) == supported_codecs.end())
      return IsNotSupported;

    if (codec == MimeUtil::H264 && ambiguous_codec_string) {
      if (video_profile == VIDEO_CODEC_PROFILE_UNKNOWN)
        video_profile = H264PROFILE_BASELINE;
      if (!IsValidH264Level(video_level))
        video_level = 10;
    } else if (codec == MimeUtil::VP9 && video_level == 0) {
      video_level = 10;
    }

     SupportsType result =
         IsCodecSupported(mime_type_lower_case, codec, video_profile,
                         video_level, color_space, is_encrypted);
     if (result == IsNotSupported)
       return IsNotSupported;
 
    if (result == MayBeSupported ||
        (result == IsSupported && ambiguous_codec_string))
      combined_result = MayBeSupported;
  }

  return combined_result;
}
