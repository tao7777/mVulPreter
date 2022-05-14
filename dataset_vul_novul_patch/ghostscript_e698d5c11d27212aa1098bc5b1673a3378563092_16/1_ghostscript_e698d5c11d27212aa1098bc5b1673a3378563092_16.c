jbig2_sd_new(Jbig2Ctx *ctx, int n_symbols)
 {
    Jbig2SymbolDict *new = NULL;
 
     if (n_symbols < 0) {
         jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "Negative number of symbols in symbol dict: %d", n_symbols);
         return NULL;
     }
 
    new = jbig2_new(ctx, Jbig2SymbolDict, 1);
    if (new != NULL) {
        new->glyphs = jbig2_new(ctx, Jbig2Image *, n_symbols);
        new->n_symbols = n_symbols;
     } else {
         jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "unable to allocate new empty symbol dict");
         return NULL;
     }
 
    if (new->glyphs != NULL) {
        memset(new->glyphs, 0, n_symbols * sizeof(Jbig2Image *));
     } else {
         jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "unable to allocate glyphs for new empty symbol dict");
        jbig2_free(ctx->allocator, new);
         return NULL;
     }
 
    return new;
 }
