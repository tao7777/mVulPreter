VideoCodec StringToVideoCodec(const std::string& codec_id) {
  std::vector<std::string> elem = base::SplitString(
      codec_id, ".", base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
  if (elem.empty())
    return kUnknownVideoCodec;
 
   VideoCodecProfile profile = VIDEO_CODEC_PROFILE_UNKNOWN;
   uint8_t level = 0;
  VideoColorSpace color_space;
 
   if (codec_id == "vp8" || codec_id == "vp8.0")
     return kCodecVP8;
  if (ParseNewStyleVp9CodecID(codec_id, &profile, &level, &color_space) ||
       ParseLegacyVp9CodecID(codec_id, &profile, &level)) {
     return kCodecVP9;
   }
  if (codec_id == "theora")
    return kCodecTheora;
  if (ParseAVCCodecId(codec_id, &profile, &level))
    return kCodecH264;
#if BUILDFLAG(ENABLE_MSE_MPEG2TS_STREAM_PARSER)
  if (ParseAVCCodecId(TranslateLegacyAvc1CodecIds(codec_id), &profile, &level))
    return kCodecH264;
#endif
#if BUILDFLAG(ENABLE_HEVC_DEMUXING)
  if (ParseHEVCCodecId(codec_id, &profile, &level))
    return kCodecHEVC;
#endif
#if BUILDFLAG(ENABLE_DOLBY_VISION_DEMUXING)
  if (ParseDolbyVisionCodecId(codec_id, &profile, &level))
    return kCodecDolbyVision;
#endif
  return kUnknownVideoCodec;
}
