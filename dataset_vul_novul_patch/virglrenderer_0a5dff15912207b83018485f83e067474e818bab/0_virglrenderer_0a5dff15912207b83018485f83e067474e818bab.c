void vrend_renderer_context_destroy(uint32_t handle)
{
   struct vrend_decode_ctx *ctx;
   bool ret;

    if (handle >= VREND_MAX_CTX)
       return;
 
   /* never destroy context 0 here, it will be destroyed in vrend_decode_reset()*/
   if (handle == 0) {
      return;
   }

    ctx = dec_ctx[handle];
    if (!ctx)
       return;
      vrend_hw_switch_context(dec_ctx[0]->grctx, true);
}
