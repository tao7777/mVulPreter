 hb_buffer_ensure_separate (hb_buffer_t *buffer, unsigned int size)
 {
  hb_buffer_ensure (buffer, size);
   if (buffer->out_info == buffer->info)
   {
     assert (buffer->have_output);
    if (!buffer->pos)
      buffer->pos = (hb_internal_glyph_position_t *) calloc (buffer->allocated, sizeof (buffer->pos[0]));
 
     buffer->out_info = (hb_internal_glyph_info_t *) buffer->pos;
     memcpy (buffer->out_info, buffer->info, buffer->out_len * sizeof (buffer->out_info[0]));
   }
 }
