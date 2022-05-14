   virtual void TearDown() {
    if (!use_high_bit_depth_) {
      vpx_free(src_);
      delete[] ref_;
#if CONFIG_VP9_HIGHBITDEPTH
    } else {
      vpx_free(CONVERT_TO_SHORTPTR(src_));
      delete[] CONVERT_TO_SHORTPTR(ref_);
#endif  // CONFIG_VP9_HIGHBITDEPTH
    }
     libvpx_test::ClearSystemState();
   }
