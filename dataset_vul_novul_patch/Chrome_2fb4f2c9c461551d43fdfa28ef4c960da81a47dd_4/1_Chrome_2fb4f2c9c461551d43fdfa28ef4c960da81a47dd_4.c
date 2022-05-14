bool MimeUtil::ParseCodecString(const std::string& mime_type_lower_case,
                                const std::string& codec_id,
                                Codec* codec,
                                 bool* ambiguous_codec_string,
                                 VideoCodecProfile* out_profile,
                                 uint8_t* out_level,
                                gfx::ColorSpace::TransferID* out_eotf) const {
   DCHECK_EQ(base::ToLowerASCII(mime_type_lower_case), mime_type_lower_case);
   DCHECK(codec);
   DCHECK(out_profile);
  DCHECK(out_level);

  *codec = INVALID_CODEC;
  *ambiguous_codec_string = false;
   *out_profile = VIDEO_CODEC_PROFILE_UNKNOWN;
   *out_level = 0;
 
  *out_eotf = gfx::ColorSpace::TransferID::BT709;
 
   std::map<std::string, Codec>::const_iterator itr =
       GetStringToCodecMap().find(codec_id);
  if (itr != GetStringToCodecMap().end()) {
    *codec = itr->second;

    return true;
  }

  if (codec_id == "avc1" || codec_id == "avc3") {
    *codec = MimeUtil::H264;
    *ambiguous_codec_string = true;
    return true;
  } else if (codec_id == "mp4a.40") {
    *codec = MimeUtil::MPEG4_AAC;
    *ambiguous_codec_string = true;
    return true;
  }

   if (ParseVp9CodecID(mime_type_lower_case, codec_id, out_profile, out_level,
                      out_eotf)) {
     *codec = MimeUtil::VP9;
     return true;
   }

  if (ParseAVCCodecId(codec_id, out_profile, out_level)) {
    *codec = MimeUtil::H264;
    *ambiguous_codec_string = !IsValidH264Level(*out_level);
    return true;
  }

#if BUILDFLAG(ENABLE_HEVC_DEMUXING)
  if (ParseHEVCCodecId(codec_id, out_profile, out_level)) {
    *codec = MimeUtil::HEVC;
    return true;
  }
#endif

#if BUILDFLAG(ENABLE_DOLBY_VISION_DEMUXING)
  if (ParseDolbyVisionCodecId(codec_id, out_profile, out_level)) {
    *codec = MimeUtil::DOLBY_VISION;
    return true;
  }
#endif

  DVLOG(2) << __func__ << ": Unrecognized codec id " << codec_id;
  return false;
}
