 hb_buffer_ensure (hb_buffer_t *buffer, unsigned int size)
 {
  if (unlikely (size > buffer->allocated))
   {
    if (unlikely (buffer->in_error))
      return FALSE;

    unsigned int new_allocated = buffer->allocated;
    hb_internal_glyph_position_t *new_pos;
    hb_internal_glyph_info_t *new_info;
    bool separate_out;

    separate_out = buffer->out_info != buffer->info;

     while (size > new_allocated)
       new_allocated += (new_allocated >> 1) + 8;
 
    new_pos = (hb_internal_glyph_position_t *) realloc (buffer->pos, new_allocated * sizeof (buffer->pos[0]));
    new_info = (hb_internal_glyph_info_t *) realloc (buffer->info, new_allocated * sizeof (buffer->info[0]));
 
    if (unlikely (!new_pos || !new_info))
      buffer->in_error = TRUE;
 
    if (likely (new_pos))
      buffer->pos = new_pos;

    if (likely (new_info))
      buffer->info = new_info;

    buffer->out_info = separate_out ? (hb_internal_glyph_info_t *) buffer->pos : buffer->info;
    if (likely (!buffer->in_error))
      buffer->allocated = new_allocated;
   }

  return likely (!buffer->in_error);
 }
