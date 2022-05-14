 VaapiVideoDecodeAccelerator::VaapiVP8Accelerator::CreateVP8Picture() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
   scoped_refptr<VaapiDecodeSurface> va_surface = vaapi_dec_->CreateSurface();
   if (!va_surface)
     return nullptr;

  return new VaapiVP8Picture(std::move(va_surface));
}
