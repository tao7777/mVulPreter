iakerb_alloc_context(iakerb_ctx_id_t *pctx)
iakerb_alloc_context(iakerb_ctx_id_t *pctx, int initiate)
 {
     iakerb_ctx_id_t ctx;
     krb5_error_code code;

    *pctx = NULL;

    ctx = k5alloc(sizeof(*ctx), &code);
    if (ctx == NULL)
        goto cleanup;
    ctx->defcred = GSS_C_NO_CREDENTIAL;
     ctx->magic = KG_IAKERB_CONTEXT;
     ctx->state = IAKERB_AS_REQ;
     ctx->count = 0;
    ctx->initiate = initiate;
    ctx->established = 0;
 
     code = krb5_gss_init_context(&ctx->k5c);
     if (code != 0)
        goto cleanup;

    *pctx = ctx;

cleanup:
    if (code != 0)
        iakerb_release_context(ctx);

    return code;
}
