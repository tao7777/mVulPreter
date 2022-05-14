     H264PictureToVaapiDecodeSurface(const scoped_refptr<H264Picture>& pic) {
   VaapiH264Picture* vaapi_pic = pic->AsVaapiH264Picture();
   CHECK(vaapi_pic);
   return vaapi_pic->dec_surface();
}
