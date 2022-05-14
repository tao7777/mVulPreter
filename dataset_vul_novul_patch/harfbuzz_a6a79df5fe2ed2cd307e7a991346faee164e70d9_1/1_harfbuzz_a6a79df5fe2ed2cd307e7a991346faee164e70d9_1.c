 hb_buffer_ensure (hb_buffer_t *buffer, unsigned int size)
 {
  unsigned int new_allocated = buffer->allocated;
  if (size > new_allocated)
   {
     while (size > new_allocated)
       new_allocated += (new_allocated >> 1) + 8;
 
    if (buffer->pos)
      buffer->pos = (hb_internal_glyph_position_t *) realloc (buffer->pos, new_allocated * sizeof (buffer->pos[0]));
 
    if (buffer->out_info != buffer->info)
    {
      buffer->info = (hb_internal_glyph_info_t *) realloc (buffer->info, new_allocated * sizeof (buffer->info[0]));
      buffer->out_info = (hb_internal_glyph_info_t *) buffer->pos;
    }
    else
    {
      buffer->info = (hb_internal_glyph_info_t *) realloc (buffer->info, new_allocated * sizeof (buffer->info[0]));
      buffer->out_info = buffer->info;
    }
 
    buffer->allocated = new_allocated;
   }
 }
