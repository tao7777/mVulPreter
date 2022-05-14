 jbig2_sd_release(Jbig2Ctx *ctx, Jbig2SymbolDict *dict)
 {
    uint32_t i;
 
     if (dict == NULL)
         return;
    for (i = 0; i < dict->n_symbols; i++)
        if (dict->glyphs[i])
            jbig2_image_release(ctx, dict->glyphs[i]);
    jbig2_free(ctx->allocator, dict->glyphs);
    jbig2_free(ctx->allocator, dict);
}
