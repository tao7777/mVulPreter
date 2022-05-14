static void vrend_renderer_init_blit_ctx(struct vrend_blitter_ctx *blit_ctx)
{
   struct virgl_gl_ctx_param ctx_params;
   int i;
   if (blit_ctx->initialised) {
      vrend_clicbs->make_current(0, blit_ctx->gl_context);
       return;
    }
 
    ctx_params.shared = true;
    ctx_params.major_ver = VREND_GL_VER_MAJOR;
    ctx_params.minor_ver = VREND_GL_VER_MINOR;

   vrend_clicbs->make_current(0, blit_ctx->gl_context);
   glGenVertexArrays(1, &blit_ctx->vaoid);
   glGenFramebuffers(1, &blit_ctx->fb_id);

   glGenBuffers(1, &blit_ctx->vbo_id);
   blit_build_vs_passthrough(blit_ctx);

   for (i = 0; i < 4; i++)
      blit_ctx->vertices[i][0][3] = 1; /*v.w*/
   glBindVertexArray(blit_ctx->vaoid);
   glBindBuffer(GL_ARRAY_BUFFER, blit_ctx->vbo_id);
}
