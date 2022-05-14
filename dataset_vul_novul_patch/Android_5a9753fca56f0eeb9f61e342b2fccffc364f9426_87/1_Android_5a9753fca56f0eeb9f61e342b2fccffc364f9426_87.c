 virtual void SetUp() {
    fwd_txfm_ = GET_PARAM(0);
    inv_txfm_ = GET_PARAM(1);

     tx_type_  = GET_PARAM(2);
     pitch_    = 8;
     fwd_txfm_ref = fdct8x8_ref;
   }
