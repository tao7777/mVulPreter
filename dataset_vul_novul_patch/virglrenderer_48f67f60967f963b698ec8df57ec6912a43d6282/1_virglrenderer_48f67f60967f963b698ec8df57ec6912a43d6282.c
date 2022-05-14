void vrend_clear(struct vrend_context *ctx,
                 unsigned buffers,
                 const union pipe_color_union *color,
                 double depth, unsigned stencil)
{
   GLbitfield bits = 0;

   if (ctx->in_error)
      return;

   if (ctx->ctx_switch_pending)
      vrend_finish_context_switch(ctx);

   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ctx->sub->fb_id);

   vrend_update_frontface_state(ctx);
   if (ctx->sub->stencil_state_dirty)
      vrend_update_stencil_state(ctx);
   if (ctx->sub->scissor_state_dirty)
      vrend_update_scissor_state(ctx);
   if (ctx->sub->viewport_state_dirty)
      vrend_update_viewport_state(ctx);

   vrend_use_program(ctx, 0);

   if (buffers & PIPE_CLEAR_COLOR) {
      if (ctx->sub->nr_cbufs && ctx->sub->surf[0] && vrend_format_is_emulated_alpha(ctx->sub->surf[0]->format)) {
         glClearColor(color->f[3], 0.0, 0.0, 0.0);
      } else {
         glClearColor(color->f[0], color->f[1], color->f[2], color->f[3]);
      }
   }

   if (buffers & PIPE_CLEAR_DEPTH) {
      /* gallium clears don't respect depth mask */
      glDepthMask(GL_TRUE);
      glClearDepth(depth);
   }

   if (buffers & PIPE_CLEAR_STENCIL)
      glClearStencil(stencil);

   if (buffers & PIPE_CLEAR_COLOR) {
      uint32_t mask = 0;
      int i;
      for (i = 0; i < ctx->sub->nr_cbufs; i++) {
         if (ctx->sub->surf[i])
            mask |= (1 << i);
      }
      if (mask != (buffers >> 2)) {
          mask = buffers >> 2;
          while (mask) {
             i = u_bit_scan(&mask);
            if (util_format_is_pure_uint(ctx->sub->surf[i]->format))
                glClearBufferuiv(GL_COLOR,
                                 i, (GLuint *)color);
            else if (util_format_is_pure_sint(ctx->sub->surf[i]->format))
                glClearBufferiv(GL_COLOR,
                                 i, (GLint *)color);
             else
               glClearBufferfv(GL_COLOR,
                                i, (GLfloat *)color);
         }
      }
      else
         bits |= GL_COLOR_BUFFER_BIT;
   }
   if (buffers & PIPE_CLEAR_DEPTH)
      bits |= GL_DEPTH_BUFFER_BIT;
   if (buffers & PIPE_CLEAR_STENCIL)
      bits |= GL_STENCIL_BUFFER_BIT;

   if (bits)
      glClear(bits);

   if (buffers & PIPE_CLEAR_DEPTH)
      if (!ctx->sub->dsa_state.depth.writemask)
         glDepthMask(GL_FALSE);
}
