     H264PictureToVaapiDecodeSurface(const scoped_refptr<H264Picture>& pic) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
   VaapiH264Picture* vaapi_pic = pic->AsVaapiH264Picture();
   CHECK(vaapi_pic);
   return vaapi_pic->dec_surface();
}
