int vrend_create_vertex_elements_state(struct vrend_context *ctx,
                                       uint32_t handle,
                                        unsigned num_elements,
                                        const struct pipe_vertex_element *elements)
 {
   struct vrend_vertex_element_array *v;
    const struct util_format_description *desc;
    GLenum type;
    int i;
    uint32_t ret_handle;
 
    if (num_elements > PIPE_MAX_ATTRIBS)
       return EINVAL;
 
   v = CALLOC_STRUCT(vrend_vertex_element_array);
   if (!v)
      return ENOMEM;

    v->count = num_elements;
    for (i = 0; i < num_elements; i++) {
       memcpy(&v->elements[i].base, &elements[i], sizeof(struct pipe_vertex_element));
      desc = util_format_description(elements[i].src_format);
      if (!desc) {
         FREE(v);
         return EINVAL;
      }

      type = GL_FALSE;
      if (desc->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
         if (desc->channel[0].size == 32)
            type = GL_FLOAT;
         else if (desc->channel[0].size == 64)
            type = GL_DOUBLE;
         else if (desc->channel[0].size == 16)
            type = GL_HALF_FLOAT;
      } else if (desc->channel[0].type == UTIL_FORMAT_TYPE_UNSIGNED &&
                 desc->channel[0].size == 8)
         type = GL_UNSIGNED_BYTE;
      else if (desc->channel[0].type == UTIL_FORMAT_TYPE_SIGNED &&
               desc->channel[0].size == 8)
         type = GL_BYTE;
      else if (desc->channel[0].type == UTIL_FORMAT_TYPE_UNSIGNED &&
               desc->channel[0].size == 16)
         type = GL_UNSIGNED_SHORT;
      else if (desc->channel[0].type == UTIL_FORMAT_TYPE_SIGNED &&
               desc->channel[0].size == 16)
         type = GL_SHORT;
      else if (desc->channel[0].type == UTIL_FORMAT_TYPE_UNSIGNED &&
               desc->channel[0].size == 32)
         type = GL_UNSIGNED_INT;
      else if (desc->channel[0].type == UTIL_FORMAT_TYPE_SIGNED &&
               desc->channel[0].size == 32)
         type = GL_INT;
      else if (elements[i].src_format == PIPE_FORMAT_R10G10B10A2_SSCALED ||
               elements[i].src_format == PIPE_FORMAT_R10G10B10A2_SNORM ||
               elements[i].src_format == PIPE_FORMAT_B10G10R10A2_SNORM)
         type = GL_INT_2_10_10_10_REV;
      else if (elements[i].src_format == PIPE_FORMAT_R10G10B10A2_USCALED ||
               elements[i].src_format == PIPE_FORMAT_R10G10B10A2_UNORM ||
               elements[i].src_format == PIPE_FORMAT_B10G10R10A2_UNORM)
         type = GL_UNSIGNED_INT_2_10_10_10_REV;
      else if (elements[i].src_format == PIPE_FORMAT_R11G11B10_FLOAT)
         type = GL_UNSIGNED_INT_10F_11F_11F_REV;

      if (type == GL_FALSE) {
         report_context_error(ctx, VIRGL_ERROR_CTX_ILLEGAL_VERTEX_FORMAT, elements[i].src_format);
         FREE(v);
         return EINVAL;
      }

      v->elements[i].type = type;
      if (desc->channel[0].normalized)
         v->elements[i].norm = GL_TRUE;
      if (desc->nr_channels == 4 && desc->swizzle[0] == UTIL_FORMAT_SWIZZLE_Z)
         v->elements[i].nr_chan = GL_BGRA;
      else if (elements[i].src_format == PIPE_FORMAT_R11G11B10_FLOAT)
         v->elements[i].nr_chan = 3;
      else
         v->elements[i].nr_chan = desc->nr_channels;
   }

   if (vrend_state.have_vertex_attrib_binding) {
      glGenVertexArrays(1, &v->id);
      glBindVertexArray(v->id);
      for (i = 0; i < num_elements; i++) {
         struct vrend_vertex_element *ve = &v->elements[i];

         if (util_format_is_pure_integer(ve->base.src_format))
            glVertexAttribIFormat(i, ve->nr_chan, ve->type, ve->base.src_offset);
         else
            glVertexAttribFormat(i, ve->nr_chan, ve->type, ve->norm, ve->base.src_offset);
         glVertexAttribBinding(i, ve->base.vertex_buffer_index);
         glVertexBindingDivisor(i, ve->base.instance_divisor);
         glEnableVertexAttribArray(i);
      }
   }
   ret_handle = vrend_renderer_object_insert(ctx, v, sizeof(struct vrend_vertex_element), handle,
                                             VIRGL_OBJECT_VERTEX_ELEMENTS);
   if (!ret_handle) {
      FREE(v);
      return ENOMEM;
   }
   return 0;
}
