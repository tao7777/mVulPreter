 hb_buffer_ensure_separate (hb_buffer_t *buffer, unsigned int size)
 {
  if (unlikely (!hb_buffer_ensure (buffer, size))) return FALSE;

   if (buffer->out_info == buffer->info)
   {
     assert (buffer->have_output);
 
     buffer->out_info = (hb_internal_glyph_info_t *) buffer->pos;
     memcpy (buffer->out_info, buffer->info, buffer->out_len * sizeof (buffer->out_info[0]));
   }

  return TRUE;
 }
