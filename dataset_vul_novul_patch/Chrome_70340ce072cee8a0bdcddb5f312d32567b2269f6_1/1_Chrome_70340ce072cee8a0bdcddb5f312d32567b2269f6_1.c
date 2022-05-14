 VaapiVideoDecodeAccelerator::VaapiVP8Accelerator::CreateVP8Picture() {
   scoped_refptr<VaapiDecodeSurface> va_surface = vaapi_dec_->CreateSurface();
   if (!va_surface)
     return nullptr;

  return new VaapiVP8Picture(std::move(va_surface));
}
