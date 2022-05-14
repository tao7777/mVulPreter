     VP8PictureToVaapiDecodeSurface(const scoped_refptr<VP8Picture>& pic) {
   VaapiVP8Picture* vaapi_pic = pic->AsVaapiVP8Picture();
   CHECK(vaapi_pic);
   return vaapi_pic->dec_surface();
}
