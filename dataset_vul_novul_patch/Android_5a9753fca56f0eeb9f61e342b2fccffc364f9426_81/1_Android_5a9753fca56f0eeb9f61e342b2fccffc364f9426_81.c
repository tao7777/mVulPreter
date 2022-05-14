 virtual void SetUp() {
    fwd_txfm_ = GET_PARAM(0);
    inv_txfm_ = GET_PARAM(1);

     tx_type_  = GET_PARAM(2);
     pitch_    = 4;
     fwd_txfm_ref = fht4x4_ref;
   }
