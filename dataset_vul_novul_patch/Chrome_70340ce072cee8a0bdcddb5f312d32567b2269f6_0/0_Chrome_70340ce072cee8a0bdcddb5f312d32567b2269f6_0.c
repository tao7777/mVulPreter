 VaapiVideoDecodeAccelerator::VaapiH264Accelerator::CreateH264Picture() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
   scoped_refptr<VaapiDecodeSurface> va_surface = vaapi_dec_->CreateSurface();
   if (!va_surface)
     return nullptr;

  return new VaapiH264Picture(std::move(va_surface));
}
