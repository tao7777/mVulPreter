SupportsType MimeUtil::IsSimpleCodecSupported(
    const std::string& mime_type_lower_case,
    Codec codec,
    bool is_encrypted) const {
  DCHECK_EQ(MimeUtilToVideoCodec(codec), kUnknownVideoCodec);
 
   SupportsType result = IsCodecSupported(
       mime_type_lower_case, codec, VIDEO_CODEC_PROFILE_UNKNOWN,
      0 /* video_level */, gfx::ColorSpace::TransferID::INVALID, is_encrypted);
 
  DCHECK_NE(result, MayBeSupported);
  return result;
}
