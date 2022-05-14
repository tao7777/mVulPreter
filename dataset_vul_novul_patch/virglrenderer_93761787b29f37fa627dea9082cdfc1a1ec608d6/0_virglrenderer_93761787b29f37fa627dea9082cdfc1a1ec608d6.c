int vrend_create_shader(struct vrend_context *ctx,
                        uint32_t handle,
                        const struct pipe_stream_output_info *so_info,
                        const char *shd_text, uint32_t offlen, uint32_t num_tokens,
                        uint32_t type, uint32_t pkt_length)
{
   struct vrend_shader_selector *sel = NULL;
   int ret_handle;
   bool new_shader = true, long_shader = false;
   bool finished = false;
   int ret;

   if (type > PIPE_SHADER_GEOMETRY)
      return EINVAL;

   if (offlen & VIRGL_OBJ_SHADER_OFFSET_CONT)
      new_shader = false;
   else if (((offlen + 3) / 4) > pkt_length)
      long_shader = true;

   /* if we have an in progress one - don't allow a new shader
      of that type or a different handle. */
   if (ctx->sub->long_shader_in_progress_handle[type]) {
      if (new_shader == true)
         return EINVAL;
      if (handle != ctx->sub->long_shader_in_progress_handle[type])
         return EINVAL;
   }

   if (new_shader) {
     sel = vrend_create_shader_state(ctx, so_info, type);
     if (sel == NULL)
       return ENOMEM;

     if (long_shader) {
        sel->buf_len = ((offlen + 3) / 4) * 4; /* round up buffer size */
        sel->tmp_buf = malloc(sel->buf_len);
        if (!sel->tmp_buf) {
           ret = ENOMEM;
           goto error;
        }
        memcpy(sel->tmp_buf, shd_text, pkt_length * 4);
        sel->buf_offset = pkt_length * 4;
        ctx->sub->long_shader_in_progress_handle[type] = handle;
     } else
        finished = true;
   } else {
      sel = vrend_object_lookup(ctx->sub->object_hash, handle, VIRGL_OBJECT_SHADER);
      if (!sel) {
         fprintf(stderr, "got continuation without original shader %d\n", handle);
         ret = EINVAL;
         goto error;
      }

      offlen &= ~VIRGL_OBJ_SHADER_OFFSET_CONT;
      if (offlen != sel->buf_offset) {
         fprintf(stderr, "Got mismatched shader continuation %d vs %d\n",
                 offlen, sel->buf_offset);
          ret = EINVAL;
          goto error;
       }

      /*make sure no overflow */
      if (pkt_length * 4 < pkt_length ||
          pkt_length * 4 + sel->buf_offset < pkt_length * 4 ||
          pkt_length * 4 + sel->buf_offset < sel->buf_offset) {
            ret = EINVAL;
            goto error;
          }

       if ((pkt_length * 4 + sel->buf_offset) > sel->buf_len) {
          fprintf(stderr, "Got too large shader continuation %d vs %d\n",
                  pkt_length * 4 + sel->buf_offset, sel->buf_len);
         shd_text = sel->tmp_buf;
      }
   }

   if (finished) {
      struct tgsi_token *tokens;

      tokens = calloc(num_tokens + 10, sizeof(struct tgsi_token));
      if (!tokens) {
         ret = ENOMEM;
         goto error;
      }

      if (vrend_dump_shaders)
         fprintf(stderr,"shader\n%s\n", shd_text);
      if (!tgsi_text_translate((const char *)shd_text, tokens, num_tokens + 10)) {
         free(tokens);
         ret = EINVAL;
         goto error;
      }

      if (vrend_finish_shader(ctx, sel, tokens)) {
         free(tokens);
         ret = EINVAL;
         goto error;
      } else {
         free(sel->tmp_buf);
         sel->tmp_buf = NULL;
      }
      free(tokens);
      ctx->sub->long_shader_in_progress_handle[type] = 0;
   }

   if (new_shader) {
      ret_handle = vrend_renderer_object_insert(ctx, sel, sizeof(*sel), handle, VIRGL_OBJECT_SHADER);
      if (ret_handle == 0) {
         ret = ENOMEM;
         goto error;
      }
   }

   return 0;

error:
   if (new_shader)
      vrend_destroy_shader_selector(sel);
   else
      vrend_renderer_object_destroy(ctx, handle);

   return ret;
}
