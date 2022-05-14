 virtual void SetUp() {
    fwd_txfm_ = GET_PARAM(0);
    inv_txfm_ = GET_PARAM(1);

     tx_type_  = GET_PARAM(2);
     pitch_    = 4;
     fwd_txfm_ref = fdct4x4_ref;
    bit_depth_ = GET_PARAM(3);
    mask_ = (1 << bit_depth_) - 1;
   }
