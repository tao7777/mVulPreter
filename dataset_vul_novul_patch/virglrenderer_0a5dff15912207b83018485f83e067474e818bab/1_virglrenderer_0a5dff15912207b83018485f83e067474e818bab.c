void vrend_renderer_context_destroy(uint32_t handle)
{
   struct vrend_decode_ctx *ctx;
   bool ret;

    if (handle >= VREND_MAX_CTX)
       return;
 
    ctx = dec_ctx[handle];
    if (!ctx)
       return;
      vrend_hw_switch_context(dec_ctx[0]->grctx, true);
}
