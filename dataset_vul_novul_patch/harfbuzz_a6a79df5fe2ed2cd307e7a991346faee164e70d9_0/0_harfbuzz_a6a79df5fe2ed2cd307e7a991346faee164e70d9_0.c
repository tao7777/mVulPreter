hb_buffer_create (unsigned int pre_alloc_size)
{
  hb_buffer_t *buffer;

  if (!HB_OBJECT_DO_CREATE (hb_buffer_t, buffer))
     return &_hb_buffer_nil;
 
   if (pre_alloc_size)
    hb_buffer_ensure (buffer, pre_alloc_size);
 
   buffer->unicode = &_hb_unicode_funcs_nil;
 
  return buffer;
}
