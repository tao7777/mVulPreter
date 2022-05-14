bool IsTransferFunctionSupported(gfx::ColorSpace::TransferID eotf) {
  switch (eotf) {
    case gfx::ColorSpace::TransferID::GAMMA22:
    case gfx::ColorSpace::TransferID::BT709:
    case gfx::ColorSpace::TransferID::SMPTE170M:
    case gfx::ColorSpace::TransferID::BT2020_10:
    case gfx::ColorSpace::TransferID::BT2020_12:
    case gfx::ColorSpace::TransferID::IEC61966_2_1:
      return true;
    default:
       return false;
   }
 }
