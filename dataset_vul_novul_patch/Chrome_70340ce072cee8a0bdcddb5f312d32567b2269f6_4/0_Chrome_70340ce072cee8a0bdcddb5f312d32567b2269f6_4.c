int VaapiVideoDecodeAccelerator::VaapiH264Accelerator::FillVARefFramesFromDPB(
     const H264DPB& dpb,
     VAPictureH264* va_pics,
     int num_pics) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
   H264Picture::Vector::const_reverse_iterator rit;
   int i;
 
  for (rit = dpb.rbegin(), i = 0; rit != dpb.rend() && i < num_pics; ++rit) {
    if ((*rit)->ref)
      FillVAPicture(&va_pics[i++], *rit);
  }

  return i;
}
