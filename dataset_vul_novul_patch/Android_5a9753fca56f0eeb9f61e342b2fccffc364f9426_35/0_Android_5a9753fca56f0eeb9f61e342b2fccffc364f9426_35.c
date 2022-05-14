  uint8_t* input() const {
  uint8_t *input() const {
#if CONFIG_VP9_HIGHBITDEPTH
    if (UUT_->use_highbd_ == 0) {
      return input_ + BorderTop() * kOuterBlockSize + BorderLeft();
    } else {
      return CONVERT_TO_BYTEPTR(input16_ + BorderTop() * kOuterBlockSize +
                                BorderLeft());
    }
#else
     return input_ + BorderTop() * kOuterBlockSize + BorderLeft();
#endif
   }
