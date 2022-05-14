void vrend_renderer_context_create_internal(uint32_t handle, uint32_t nlen,
                                            const char *debug_name)
{
   struct vrend_decode_ctx *dctx;

    if (handle >= VREND_MAX_CTX)
       return;
 
   dctx = dec_ctx[handle];
   if (dctx)
      return;

    dctx = malloc(sizeof(struct vrend_decode_ctx));
    if (!dctx)
       return;
      return;
   }
