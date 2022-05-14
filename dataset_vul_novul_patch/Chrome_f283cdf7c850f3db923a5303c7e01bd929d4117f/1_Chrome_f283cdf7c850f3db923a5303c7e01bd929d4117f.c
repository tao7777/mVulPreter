bool VaapiJpegDecoder::Initialize(const base::RepeatingClosure& error_uma_cb) {
  vaapi_wrapper_ = VaapiWrapper::Create(VaapiWrapper::kDecode,
                                        VAProfileJPEGBaseline, error_uma_cb);
  if (!vaapi_wrapper_) {
    VLOGF(1) << "Failed initializing VAAPI";
    return false;
  }
  return true;
}
