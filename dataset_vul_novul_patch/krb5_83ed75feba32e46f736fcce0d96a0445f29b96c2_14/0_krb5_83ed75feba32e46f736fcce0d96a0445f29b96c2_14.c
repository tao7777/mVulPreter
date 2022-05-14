 generic_ret *init_2_svc(krb5_ui_4 *arg, struct svc_req *rqstp)
 {
     static generic_ret         ret;
    gss_buffer_desc            client_name = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc            service_name = GSS_C_EMPTY_BUFFER;
     kadm5_server_handle_t      handle;
     OM_uint32                  minor_stat;
     const char                 *errmsg = NULL;
    size_t clen, slen;
    char *cdots, *sdots;

    xdr_free(xdr_generic_ret, &ret);

    if ((ret.code = new_server_handle(*arg, rqstp, &handle)))
        goto exit_func;
    if (! (ret.code = check_handle((void *)handle))) {
        ret.api_version = handle->api_version;
    }

    free_server_handle(handle);

    if (setup_gss_names(rqstp, &client_name, &service_name) < 0) {
        ret.code = KADM5_FAILURE;
        goto exit_func;
    }

    if (ret.code != 0)
        errmsg = krb5_get_error_message(NULL, ret.code);

    clen = client_name.length;
    trunc_name(&clen, &cdots);
    slen = service_name.length;
    trunc_name(&slen, &sdots);
    /* okay to cast lengths to int because trunc_name limits max value */
    krb5_klog_syslog(LOG_NOTICE, _("Request: kadm5_init, %.*s%s, %s, "
                                   "client=%.*s%s, service=%.*s%s, addr=%s, "
                                   "vers=%d, flavor=%d"),
                     (int)clen, (char *)client_name.value, cdots,
                     errmsg ? errmsg : _("success"),
                     (int)clen, (char *)client_name.value, cdots,
                     (int)slen, (char *)service_name.value, sdots,
                     client_addr(rqstp->rq_xprt),
                     ret.api_version & ~(KADM5_API_VERSION_MASK),
                      rqstp->rq_cred.oa_flavor);
     if (errmsg != NULL)
         krb5_free_error_message(NULL, errmsg);
 
 exit_func:
    gss_release_buffer(&minor_stat, &client_name);
    gss_release_buffer(&minor_stat, &service_name);
     return(&ret);
 }
