 void vrend_set_framebuffer_state(struct vrend_context *ctx,
                                 uint32_t nr_cbufs, uint32_t surf_handle[8],
                                  uint32_t zsurf_handle)
 {
    struct vrend_surface *surf, *zsurf;
   int i;
   int old_num;
   GLenum status;
   GLint new_height = -1;
   bool new_ibf = false;

   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ctx->sub->fb_id);

   if (zsurf_handle) {
      zsurf = vrend_object_lookup(ctx->sub->object_hash, zsurf_handle, VIRGL_OBJECT_SURFACE);
      if (!zsurf) {
         report_context_error(ctx, VIRGL_ERROR_CTX_ILLEGAL_SURFACE, zsurf_handle);
         return;
      }
   } else
      zsurf = NULL;

   if (ctx->sub->zsurf != zsurf) {
      vrend_surface_reference(&ctx->sub->zsurf, zsurf);
      vrend_hw_set_zsurf_texture(ctx);
   }

   old_num = ctx->sub->nr_cbufs;
   ctx->sub->nr_cbufs = nr_cbufs;
   ctx->sub->old_nr_cbufs = old_num;

   for (i = 0; i < nr_cbufs; i++) {
      if (surf_handle[i] != 0) {
         surf = vrend_object_lookup(ctx->sub->object_hash, surf_handle[i], VIRGL_OBJECT_SURFACE);
         if (!surf) {
            report_context_error(ctx, VIRGL_ERROR_CTX_ILLEGAL_SURFACE, surf_handle[i]);
            return;
         }
      } else
         surf = NULL;

      if (ctx->sub->surf[i] != surf) {
         vrend_surface_reference(&ctx->sub->surf[i], surf);
         vrend_hw_set_color_surface(ctx, i);
      }
   }

   if (old_num > ctx->sub->nr_cbufs) {
      for (i = ctx->sub->nr_cbufs; i < old_num; i++) {
         vrend_surface_reference(&ctx->sub->surf[i], NULL);
         vrend_hw_set_color_surface(ctx, i);
      }
   }

   /* find a buffer to set fb_height from */
   if (ctx->sub->nr_cbufs == 0 && !ctx->sub->zsurf) {
      new_height = 0;
      new_ibf = false;
   } else if (ctx->sub->nr_cbufs == 0) {
      new_height = u_minify(ctx->sub->zsurf->texture->base.height0, ctx->sub->zsurf->val0);
      new_ibf = ctx->sub->zsurf->texture->y_0_top ? true : false;
   }
   else {
      surf = NULL;
      for (i = 0; i < ctx->sub->nr_cbufs; i++) {
         if (ctx->sub->surf[i]) {
            surf = ctx->sub->surf[i];
            break;
         }
      }
      if (surf == NULL) {
         report_context_error(ctx, VIRGL_ERROR_CTX_ILLEGAL_SURFACE, i);
         return;
      }
      new_height = u_minify(surf->texture->base.height0, surf->val0);
      new_ibf = surf->texture->y_0_top ? true : false;
   }

   if (new_height != -1) {
      if (ctx->sub->fb_height != new_height || ctx->sub->inverted_fbo_content != new_ibf) {
         ctx->sub->fb_height = new_height;
         ctx->sub->inverted_fbo_content = new_ibf;
         ctx->sub->scissor_state_dirty = (1 << 0);
         ctx->sub->viewport_state_dirty = (1 << 0);
      }
   }

   vrend_hw_emit_framebuffer_state(ctx);

   if (ctx->sub->nr_cbufs > 0 || ctx->sub->zsurf) {
      status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE)
         fprintf(stderr,"failed to complete framebuffer 0x%x %s\n", status, ctx->debug_name);
   }
   ctx->sub->shader_dirty = true;
}
