bool IsTransferFunctionSupported(gfx::ColorSpace::TransferID eotf) {
bool IsColorSpaceSupported(const media::VideoColorSpace& color_space) {
  bool color_management =
      base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableHDROutput) ||
      base::FeatureList::IsEnabled(media::kVideoColorManagement);
  switch (color_space.primaries) {
    case media::VideoColorSpace::PrimaryID::EBU_3213_E:
    case media::VideoColorSpace::PrimaryID::INVALID:
      return false;

    case media::VideoColorSpace::PrimaryID::BT709:
    case media::VideoColorSpace::PrimaryID::UNSPECIFIED:
    case media::VideoColorSpace::PrimaryID::BT470M:
    case media::VideoColorSpace::PrimaryID::BT470BG:
    case media::VideoColorSpace::PrimaryID::SMPTE170M:
      break;

    // Supported with color management.
    case media::VideoColorSpace::PrimaryID::SMPTE240M:
    case media::VideoColorSpace::PrimaryID::FILM:
    case media::VideoColorSpace::PrimaryID::BT2020:
    case media::VideoColorSpace::PrimaryID::SMPTEST428_1:
    case media::VideoColorSpace::PrimaryID::SMPTEST431_2:
    case media::VideoColorSpace::PrimaryID::SMPTEST432_1:
      if (!color_management)
        return false;
      break;
  }

  switch (color_space.transfer) {
    // Transfers supported without color management.
    case media::VideoColorSpace::TransferID::UNSPECIFIED:
    case media::VideoColorSpace::TransferID::GAMMA22:
    case media::VideoColorSpace::TransferID::BT709:
    case media::VideoColorSpace::TransferID::SMPTE170M:
    case media::VideoColorSpace::TransferID::BT2020_10:
    case media::VideoColorSpace::TransferID::BT2020_12:
    case media::VideoColorSpace::TransferID::IEC61966_2_1:
      break;

    // Supported with color management.
    case media::VideoColorSpace::TransferID::GAMMA28:
    case media::VideoColorSpace::TransferID::SMPTE240M:
    case media::VideoColorSpace::TransferID::LINEAR:
    case media::VideoColorSpace::TransferID::LOG:
    case media::VideoColorSpace::TransferID::LOG_SQRT:
    case media::VideoColorSpace::TransferID::BT1361_ECG:
    case media::VideoColorSpace::TransferID::SMPTEST2084:
    case media::VideoColorSpace::TransferID::IEC61966_2_4:
    case media::VideoColorSpace::TransferID::SMPTEST428_1:
    case media::VideoColorSpace::TransferID::ARIB_STD_B67:
      if (!color_management)
        return false;
      break;

    // Never supported.
    case media::VideoColorSpace::TransferID::INVALID:
       return false;
   }

  switch (color_space.matrix) {
    // Supported without color management.
    case media::VideoColorSpace::MatrixID::BT709:
    case media::VideoColorSpace::MatrixID::UNSPECIFIED:
    case media::VideoColorSpace::MatrixID::BT470BG:
    case media::VideoColorSpace::MatrixID::SMPTE170M:
    case media::VideoColorSpace::MatrixID::BT2020_NCL:
      break;

    // Supported with color management.
    case media::VideoColorSpace::MatrixID::RGB:
    case media::VideoColorSpace::MatrixID::FCC:
    case media::VideoColorSpace::MatrixID::SMPTE240M:
    case media::VideoColorSpace::MatrixID::YCOCG:
    case media::VideoColorSpace::MatrixID::YDZDX:
    case media::VideoColorSpace::MatrixID::BT2020_CL:
      if (!color_management)
        return false;
      break;

    // Never supported.
    case media::VideoColorSpace::MatrixID::INVALID:
      return false;
  }

  if (color_space.range == gfx::ColorSpace::RangeID::INVALID)
    return false;

  return true;
 }
