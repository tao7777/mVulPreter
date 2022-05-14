 bool VaapiVideoDecodeAccelerator::VaapiH264Accelerator::SubmitDecode(
     const scoped_refptr<H264Picture>& pic) {
   VLOGF(4) << "Decoding POC " << pic->pic_order_cnt;
   scoped_refptr<VaapiDecodeSurface> dec_surface =
       H264PictureToVaapiDecodeSurface(pic);
 
  return vaapi_dec_->DecodeSurface(dec_surface);
}
