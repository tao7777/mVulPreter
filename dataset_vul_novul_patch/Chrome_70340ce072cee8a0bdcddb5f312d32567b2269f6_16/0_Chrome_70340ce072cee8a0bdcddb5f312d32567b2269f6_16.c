     VP8PictureToVaapiDecodeSurface(const scoped_refptr<VP8Picture>& pic) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
   VaapiVP8Picture* vaapi_pic = pic->AsVaapiVP8Picture();
   CHECK(vaapi_pic);
   return vaapi_pic->dec_surface();
}
