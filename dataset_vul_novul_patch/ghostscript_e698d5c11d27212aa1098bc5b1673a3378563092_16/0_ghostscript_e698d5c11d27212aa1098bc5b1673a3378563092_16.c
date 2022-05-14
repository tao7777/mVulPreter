jbig2_sd_new(Jbig2Ctx *ctx, int n_symbols)
jbig2_sd_new(Jbig2Ctx *ctx, uint32_t n_symbols)
 {
    Jbig2SymbolDict *new_dict = NULL;
 
     if (n_symbols < 0) {
         jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "Negative number of symbols in symbol dict: %d", n_symbols);
         return NULL;
     }
 
    new_dict = jbig2_new(ctx, Jbig2SymbolDict, 1);
    if (new_dict != NULL) {
        new_dict->glyphs = jbig2_new(ctx, Jbig2Image *, n_symbols);
        new_dict->n_symbols = n_symbols;
     } else {
         jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "unable to allocate new empty symbol dict");
         return NULL;
     }
 
    if (new_dict->glyphs != NULL) {
        memset(new_dict->glyphs, 0, n_symbols * sizeof(Jbig2Image *));
     } else {
         jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "unable to allocate glyphs for new empty symbol dict");
        jbig2_free(ctx->allocator, new_dict);
         return NULL;
     }
 
    return new_dict;
 }
