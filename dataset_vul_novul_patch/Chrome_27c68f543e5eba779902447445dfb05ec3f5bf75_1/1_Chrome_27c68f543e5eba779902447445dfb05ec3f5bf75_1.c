bool VaapiVideoDecodeAccelerator::VaapiVP9Accelerator::OutputPicture(
    const scoped_refptr<VP9Picture>& pic) {
  scoped_refptr<VaapiDecodeSurface> dec_surface =
      VP9PictureToVaapiDecodeSurface(pic);
  vaapi_dec_->SurfaceReady(dec_surface);
  return true;
}
