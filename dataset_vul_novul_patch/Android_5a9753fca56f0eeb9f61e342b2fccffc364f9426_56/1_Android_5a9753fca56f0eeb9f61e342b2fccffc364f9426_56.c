   virtual void SetUp() {
    fwd_txfm_ = GET_PARAM(0);

     inv_txfm_ = GET_PARAM(1);
     version_  = GET_PARAM(2);  // 0: high precision forward transform
   }
