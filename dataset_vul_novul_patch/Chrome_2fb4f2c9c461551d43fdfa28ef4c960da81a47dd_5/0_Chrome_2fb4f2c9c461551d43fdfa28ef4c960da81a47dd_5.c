static bool ParseVp9CodecID(const std::string& mime_type_lower_case,
                             const std::string& codec_id,
                             VideoCodecProfile* out_profile,
                             uint8_t* out_level,
                            VideoColorSpace* out_color_space) {
   if (mime_type_lower_case == "video/mp4") {
     if (base::CommandLine::ForCurrentProcess()->HasSwitch(
             switches::kEnableVp9InMp4)) {
       return ParseNewStyleVp9CodecID(codec_id, out_profile, out_level,
                                     out_color_space);
     }
   } else if (mime_type_lower_case == "video/webm") {
     if (HasNewVp9CodecStringSupport() &&
        ParseNewStyleVp9CodecID(codec_id, out_profile, out_level,
                                out_color_space)) {
       return true;
     }
 
    return ParseLegacyVp9CodecID(codec_id, out_profile, out_level);
  }
  return false;
}
