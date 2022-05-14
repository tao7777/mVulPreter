 push_decoder_state (DECODER_STATE ds)
 {
   if (ds->idx >= ds->stacksize)
     {
      fprintf (stderr, "ksba: ber-decoder: stack overflow!\n");
      return gpg_error (GPG_ERR_LIMIT_REACHED);
     }
   ds->stack[ds->idx++] = ds->cur;
  return 0;
 }
