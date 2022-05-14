 VaapiVideoDecodeAccelerator::VaapiH264Accelerator::CreateH264Picture() {
   scoped_refptr<VaapiDecodeSurface> va_surface = vaapi_dec_->CreateSurface();
   if (!va_surface)
     return nullptr;

  return new VaapiH264Picture(std::move(va_surface));
}
