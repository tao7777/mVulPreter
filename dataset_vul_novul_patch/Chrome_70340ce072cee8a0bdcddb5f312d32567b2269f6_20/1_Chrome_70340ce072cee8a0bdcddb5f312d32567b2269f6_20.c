VaapiVideoDecodeAccelerator::VaapiVP9Accelerator::VaapiVP9Accelerator(
    VaapiVideoDecodeAccelerator* vaapi_dec,
    VaapiWrapper* vaapi_wrapper)
     : vaapi_wrapper_(vaapi_wrapper), vaapi_dec_(vaapi_dec) {
   DCHECK(vaapi_wrapper_);
   DCHECK(vaapi_dec_);
 }
