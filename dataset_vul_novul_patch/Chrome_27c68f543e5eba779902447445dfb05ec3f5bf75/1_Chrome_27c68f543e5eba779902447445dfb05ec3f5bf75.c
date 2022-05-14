VaapiVideoDecodeAccelerator::VaapiVP9Accelerator::CreateVP9Picture() {
  scoped_refptr<VaapiDecodeSurface> va_surface = vaapi_dec_->CreateSurface();
  if (!va_surface)
    return nullptr;
  return new VaapiVP9Picture(va_surface);
}
