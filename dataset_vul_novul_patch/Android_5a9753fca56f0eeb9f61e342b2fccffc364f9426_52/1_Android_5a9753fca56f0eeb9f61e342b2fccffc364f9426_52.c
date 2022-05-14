 virtual void SetUp() {

     fwd_txfm_ = GET_PARAM(0);
     inv_txfm_ = GET_PARAM(1);
     tx_type_  = GET_PARAM(2);
     pitch_    = 16;
     fwd_txfm_ref = fdct16x16_ref;
   }
