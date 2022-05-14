 void VaapiVideoDecodeAccelerator::VaapiH264Accelerator::Reset() {
   vaapi_wrapper_->DestroyPendingBuffers();
 }
