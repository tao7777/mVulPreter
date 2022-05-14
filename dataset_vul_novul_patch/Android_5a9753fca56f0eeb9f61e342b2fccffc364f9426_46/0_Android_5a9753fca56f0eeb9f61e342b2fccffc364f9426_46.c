  void RunFwdTxfm(int16_t *in, int16_t *out, int stride) {
  void RunFwdTxfm(int16_t *in, tran_low_t *out, int stride) {
     fwd_txfm_(in, out, stride);
   }
