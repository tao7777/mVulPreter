     VP9PictureToVaapiDecodeSurface(const scoped_refptr<VP9Picture>& pic) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
   VaapiVP9Picture* vaapi_pic = pic->AsVaapiVP9Picture();
   CHECK(vaapi_pic);
   return vaapi_pic->dec_surface();
}
