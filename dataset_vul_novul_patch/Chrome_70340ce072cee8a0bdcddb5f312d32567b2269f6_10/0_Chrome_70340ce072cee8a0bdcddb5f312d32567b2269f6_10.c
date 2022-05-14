 void VaapiVideoDecodeAccelerator::VaapiH264Accelerator::Reset() {
  DETACH_FROM_SEQUENCE(sequence_checker_);
   vaapi_wrapper_->DestroyPendingBuffers();
 }
