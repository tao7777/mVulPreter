 bool VaapiVideoDecodeAccelerator::VaapiVP8Accelerator::OutputPicture(
     const scoped_refptr<VP8Picture>& pic) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
   scoped_refptr<VaapiDecodeSurface> dec_surface =
       VP8PictureToVaapiDecodeSurface(pic);
   dec_surface->set_visible_rect(pic->visible_rect);
  vaapi_dec_->SurfaceReady(dec_surface);
  return true;
}
