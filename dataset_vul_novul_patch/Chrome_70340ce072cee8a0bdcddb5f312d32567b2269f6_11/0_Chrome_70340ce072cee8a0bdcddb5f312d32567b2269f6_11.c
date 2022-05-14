 bool VaapiVideoDecodeAccelerator::VaapiH264Accelerator::SubmitDecode(
     const scoped_refptr<H264Picture>& pic) {
   VLOGF(4) << "Decoding POC " << pic->pic_order_cnt;
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
   scoped_refptr<VaapiDecodeSurface> dec_surface =
       H264PictureToVaapiDecodeSurface(pic);
 
  return vaapi_dec_->DecodeSurface(dec_surface);
}
