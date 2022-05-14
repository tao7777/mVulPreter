static int vrend_decode_create_ve(struct vrend_decode_ctx *ctx, uint32_t handle, uint16_t length)
{
   struct pipe_vertex_element *ve = NULL;
   int num_elements;
   int i;
   int ret;

   if (length < 1)
      return EINVAL;

   if ((length - 1) % 4)
      return EINVAL;

   num_elements = (length - 1) / 4;

   if (num_elements) {
      ve = calloc(num_elements, sizeof(struct pipe_vertex_element));

      if (!ve)
         return ENOMEM;

      for (i = 0; i < num_elements; i++) {
          ve[i].src_offset = get_buf_entry(ctx, VIRGL_OBJ_VERTEX_ELEMENTS_V0_SRC_OFFSET(i));
          ve[i].instance_divisor = get_buf_entry(ctx, VIRGL_OBJ_VERTEX_ELEMENTS_V0_INSTANCE_DIVISOR(i));
          ve[i].vertex_buffer_index = get_buf_entry(ctx, VIRGL_OBJ_VERTEX_ELEMENTS_V0_VERTEX_BUFFER_INDEX(i));

         if (ve[i].vertex_buffer_index >= PIPE_MAX_ATTRIBS)
            return EINVAL;

          ve[i].src_format = get_buf_entry(ctx, VIRGL_OBJ_VERTEX_ELEMENTS_V0_SRC_FORMAT(i));
       }
    }
   return ret;
}
