 bool RenderMediaClient::IsSupportedVideoConfig(
    const media::VideoConfig& config) {
   switch (config.codec) {
     case media::kCodecVP9:
      return IsColorSpaceSupported(config.color_space);
 
     case media::kCodecH264:
     case media::kCodecVP8:
    case media::kCodecTheora:
      return true;

    case media::kUnknownVideoCodec:
    case media::kCodecVC1:
    case media::kCodecMPEG2:
    case media::kCodecMPEG4:
    case media::kCodecHEVC:
    case media::kCodecDolbyVision:
      return false;
  }

  NOTREACHED();
  return false;
}
