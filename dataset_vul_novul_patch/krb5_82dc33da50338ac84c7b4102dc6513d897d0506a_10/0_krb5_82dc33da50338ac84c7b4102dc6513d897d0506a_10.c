krb5_gss_wrap_size_limit(minor_status, context_handle, conf_req_flag,
                         qop_req, req_output_size, max_input_size)
    OM_uint32           *minor_status;
    gss_ctx_id_t        context_handle;
    int                 conf_req_flag;
    gss_qop_t           qop_req;
    OM_uint32           req_output_size;
    OM_uint32           *max_input_size;
{
    krb5_gss_ctx_id_rec *ctx;
    OM_uint32           data_size, conflen;
    OM_uint32           ohlen;
    int                 overhead;

    /* only default qop is allowed */
    if (qop_req != GSS_C_QOP_DEFAULT) {
        *minor_status = (OM_uint32) G_UNKNOWN_QOP;
        return(GSS_S_FAILURE);
     }
 
     ctx = (krb5_gss_ctx_id_rec *) context_handle;
    if (ctx->terminated || !ctx->established) {
         *minor_status = KG_CTX_INCOMPLETE;
         return(GSS_S_NO_CONTEXT);
     }

    if (ctx->proto == 1) {
        /* No pseudo-ASN.1 wrapper overhead, so no sequence length and
           OID.  */
        OM_uint32 sz = req_output_size;

        /* Token header: 16 octets.  */
        if (conf_req_flag) {
            krb5_key key;
            krb5_enctype enctype;

            key = ctx->have_acceptor_subkey ? ctx->acceptor_subkey
                : ctx->subkey;
            enctype = key->keyblock.enctype;

            while (sz > 0 && krb5_encrypt_size(sz, enctype) + 16 > req_output_size)
                sz--;
            /* Allow for encrypted copy of header.  */
            if (sz > 16)
                sz -= 16;
            else
                sz = 0;
#ifdef CFX_EXERCISE
            /* Allow for EC padding.  In the MIT implementation, only
               added while testing.  */
            if (sz > 65535)
                sz -= 65535;
            else
                sz = 0;
#endif
        } else {
            krb5_cksumtype cksumtype;
            krb5_error_code err;
            size_t cksumsize;

            cksumtype = ctx->have_acceptor_subkey ? ctx->acceptor_subkey_cksumtype
                : ctx->cksumtype;

            err = krb5_c_checksum_length(ctx->k5_context, cksumtype, &cksumsize);
            if (err) {
                *minor_status = err;
                return GSS_S_FAILURE;
            }

            /* Allow for token header and checksum.  */
            if (sz < 16 + cksumsize)
                sz = 0;
            else
                sz -= (16 + cksumsize);
        }

        *max_input_size = sz;
        *minor_status = 0;
        return GSS_S_COMPLETE;
    }

    /* Calculate the token size and subtract that from the output size */
    overhead = 7 + ctx->mech_used->length;
    data_size = req_output_size;
    conflen = kg_confounder_size(ctx->k5_context, ctx->enc->keyblock.enctype);
    data_size = (conflen + data_size + 8) & (~(OM_uint32)7);
    ohlen = g_token_size(ctx->mech_used,
                         (unsigned int) (data_size + ctx->cksum_size + 14))
        - req_output_size;

    if (ohlen+overhead < req_output_size)
        /*
         * Cannot have trailer length that will cause us to pad over our
         * length.
         */
        *max_input_size = (req_output_size - ohlen - overhead) & (~(OM_uint32)7);
    else
        *max_input_size = 0;

    *minor_status = 0;
    return(GSS_S_COMPLETE);
}
