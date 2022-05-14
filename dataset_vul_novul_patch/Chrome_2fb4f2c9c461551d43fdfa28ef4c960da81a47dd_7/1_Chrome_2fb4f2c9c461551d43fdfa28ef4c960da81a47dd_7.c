bool ParseVp9Eotf(const int value, gfx::ColorSpace::TransferID* eotf) {
  switch (value) {
    case 1:
      *eotf = gfx::ColorSpace::TransferID::BT709;
      break;
    case 2:
      *eotf = gfx::ColorSpace::TransferID::INVALID;
      break;
    case 4:
      *eotf = gfx::ColorSpace::TransferID::GAMMA22;
      break;
    case 5:
      *eotf = gfx::ColorSpace::TransferID::GAMMA28;
      break;
    case 6:
      *eotf = gfx::ColorSpace::TransferID::SMPTE170M;
      break;
    case 7:
      *eotf = gfx::ColorSpace::TransferID::SMPTE240M;
      break;
    case 8:
      *eotf = gfx::ColorSpace::TransferID::LINEAR;
      break;
    case 9:
      *eotf = gfx::ColorSpace::TransferID::LOG;
      break;
    case 10:
      *eotf = gfx::ColorSpace::TransferID::LOG_SQRT;
      break;
    case 11:
      *eotf = gfx::ColorSpace::TransferID::IEC61966_2_4;
      break;
    case 12:
      *eotf = gfx::ColorSpace::TransferID::BT1361_ECG;
      break;
    case 13:
      *eotf = gfx::ColorSpace::TransferID::IEC61966_2_1;
      break;
    case 14:
      *eotf = gfx::ColorSpace::TransferID::BT2020_10;
      break;
    case 15:
      *eotf = gfx::ColorSpace::TransferID::BT2020_12;
      break;
    case 16:
      *eotf = gfx::ColorSpace::TransferID::SMPTEST2084;
      break;
    case 17:
      *eotf = gfx::ColorSpace::TransferID::SMPTEST428_1;
      break;
    default:
      *eotf = gfx::ColorSpace::TransferID::INVALID;
      return false;
  }
  return true;
}
