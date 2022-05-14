hb_buffer_clear (hb_buffer_t *buffer)
 {
   buffer->have_output = FALSE;
   buffer->have_positions = FALSE;
  buffer->in_error = FALSE;
   buffer->len = 0;
   buffer->out_len = 0;
   buffer->i = 0;
  buffer->max_lig_id = 0;
   buffer->max_lig_id = 0;
 }
