  void RunInvTxfm(int16_t *out, uint8_t *dst, int stride) {
  void RunInvTxfm(tran_low_t *out, uint8_t *dst, int stride) {
     inv_txfm_(out, dst, stride, tx_type_);
   }
