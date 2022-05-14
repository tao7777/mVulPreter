 pop_decoder_state (DECODER_STATE ds)
 {
   if (!ds->idx)
     {
      fprintf (stderr, "ksba: ber-decoder: stack underflow!\n");
      return gpg_error (GPG_ERR_INTERNAL);
     }
   ds->cur = ds->stack[--ds->idx];
  return 0;
 }
