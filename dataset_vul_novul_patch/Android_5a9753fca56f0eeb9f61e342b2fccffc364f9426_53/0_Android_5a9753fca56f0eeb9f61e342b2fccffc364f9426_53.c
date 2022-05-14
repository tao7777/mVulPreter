 virtual void SetUp() {

     fwd_txfm_ = GET_PARAM(0);
     inv_txfm_ = GET_PARAM(1);
     tx_type_  = GET_PARAM(2);
    bit_depth_ = GET_PARAM(3);
     pitch_    = 16;
     fwd_txfm_ref = fht16x16_ref;
    inv_txfm_ref = iht16x16_ref;
    mask_ = (1 << bit_depth_) - 1;
#if CONFIG_VP9_HIGHBITDEPTH
    switch (bit_depth_) {
      case VPX_BITS_10:
        inv_txfm_ref = iht16x16_10;
        break;
      case VPX_BITS_12:
        inv_txfm_ref = iht16x16_12;
        break;
      default:
        inv_txfm_ref = iht16x16_ref;
        break;
    }
#else
    inv_txfm_ref = iht16x16_ref;
#endif
   }
