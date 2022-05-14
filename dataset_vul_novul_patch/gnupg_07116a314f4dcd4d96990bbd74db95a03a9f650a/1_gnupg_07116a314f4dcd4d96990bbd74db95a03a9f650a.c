 pop_decoder_state (DECODER_STATE ds)
 {
   if (!ds->idx)
     {
      fprintf (stderr, "ERROR: decoder stack underflow!\n");
      abort ();
     }
   ds->cur = ds->stack[--ds->idx];
 }
