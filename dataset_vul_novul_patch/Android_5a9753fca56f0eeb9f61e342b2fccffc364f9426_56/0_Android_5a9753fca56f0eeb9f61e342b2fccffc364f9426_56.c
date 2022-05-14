   virtual void SetUp() {
    fwd_txfm_ = GET_PARAM(0);

     inv_txfm_ = GET_PARAM(1);
     version_  = GET_PARAM(2);  // 0: high precision forward transform
    bit_depth_ = GET_PARAM(3);
    mask_ = (1 << bit_depth_) - 1;
   }
