krb5_gss_inquire_context(minor_status, context_handle, initiator_name,
                         acceptor_name, lifetime_rec, mech_type, ret_flags,
                         locally_initiated, opened)
    OM_uint32 *minor_status;
    gss_ctx_id_t context_handle;
    gss_name_t *initiator_name;
    gss_name_t *acceptor_name;
    OM_uint32 *lifetime_rec;
    gss_OID *mech_type;
    OM_uint32 *ret_flags;
    int *locally_initiated;
    int *opened;
{
    krb5_context context;
    krb5_error_code code;
    krb5_gss_ctx_id_rec *ctx;
    krb5_gss_name_t initiator, acceptor;
    krb5_timestamp now;
    krb5_deltat lifetime;

    if (initiator_name)
        *initiator_name = (gss_name_t) NULL;
    if (acceptor_name)
        *acceptor_name = (gss_name_t) NULL;
 
     ctx = (krb5_gss_ctx_id_rec *) context_handle;
 
    if (ctx->terminated || !ctx->established) {
         *minor_status = KG_CTX_INCOMPLETE;
         return(GSS_S_NO_CONTEXT);
     }

    initiator = NULL;
    acceptor = NULL;
    context = ctx->k5_context;

    if ((code = krb5_timeofday(context, &now))) {
        *minor_status = code;
        save_error_info(*minor_status, context);
        return(GSS_S_FAILURE);
    }

    if ((lifetime = ctx->krb_times.endtime - now) < 0)
        lifetime = 0;

    if (initiator_name) {
        if ((code = kg_duplicate_name(context,
                                      ctx->initiate ? ctx->here : ctx->there,
                                      &initiator))) {
            *minor_status = code;
            save_error_info(*minor_status, context);
            return(GSS_S_FAILURE);
        }
    }

    if (acceptor_name) {
        if ((code = kg_duplicate_name(context,
                                      ctx->initiate ? ctx->there : ctx->here,
                                      &acceptor))) {
            if (initiator)
                kg_release_name(context, &initiator);
            *minor_status = code;
            save_error_info(*minor_status, context);
            return(GSS_S_FAILURE);
        }
    }

    if (initiator_name)
        *initiator_name = (gss_name_t) initiator;

    if (acceptor_name)
        *acceptor_name = (gss_name_t) acceptor;

    if (lifetime_rec)
        *lifetime_rec = lifetime;

    if (mech_type)
        *mech_type = (gss_OID) ctx->mech_used;

    if (ret_flags)
        *ret_flags = ctx->gss_flags;

    if (locally_initiated)
        *locally_initiated = ctx->initiate;

    if (opened)
        *opened = ctx->established;

    *minor_status = 0;
    return((lifetime == 0)?GSS_S_CONTEXT_EXPIRED:GSS_S_COMPLETE);
}
