    VP9PictureToVaapiDecodeSurface(const scoped_refptr<VP9Picture>& pic) {
  VaapiVP9Picture* vaapi_pic = pic->AsVaapiVP9Picture();
  CHECK(vaapi_pic);
  return vaapi_pic->dec_surface();
}
