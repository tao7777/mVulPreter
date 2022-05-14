krb5_gss_context_time(minor_status, context_handle, time_rec)
    OM_uint32 *minor_status;
    gss_ctx_id_t context_handle;
    OM_uint32 *time_rec;
{
    krb5_error_code code;
    krb5_gss_ctx_id_rec *ctx;
    krb5_timestamp now;
    krb5_deltat lifetime;
 
     ctx = (krb5_gss_ctx_id_rec *) context_handle;
 
    if (ctx->terminated || !ctx->established) {
         *minor_status = KG_CTX_INCOMPLETE;
         return(GSS_S_NO_CONTEXT);
     }

    if ((code = krb5_timeofday(ctx->k5_context, &now))) {
        *minor_status = code;
        save_error_info(*minor_status, ctx->k5_context);
        return(GSS_S_FAILURE);
    }

    if ((lifetime = ctx->krb_times.endtime - now) <= 0) {
        *time_rec = 0;
        *minor_status = 0;
        return(GSS_S_CONTEXT_EXPIRED);
    } else {
        *time_rec = lifetime;
        *minor_status = 0;
        return(GSS_S_COMPLETE);
    }
}
