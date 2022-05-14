 push_decoder_state (DECODER_STATE ds)
 {
   if (ds->idx >= ds->stacksize)
     {
      fprintf (stderr, "ERROR: decoder stack overflow!\n");
      abort ();
     }
   ds->stack[ds->idx++] = ds->cur;
 }
