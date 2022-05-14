get_pols_2_svc(gpols_arg *arg, struct svc_req *rqstp)
 {
     static gpols_ret                ret;
     char                            *prime_arg;
    gss_buffer_desc                 client_name = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc                 service_name = GSS_C_EMPTY_BUFFER;
     OM_uint32                       minor_stat;
     kadm5_server_handle_t           handle;
     const char                      *errmsg = NULL;

    xdr_free(xdr_gpols_ret, &ret);

    if ((ret.code = new_server_handle(arg->api_version, rqstp, &handle)))
        goto exit_func;

    if ((ret.code = check_handle((void *)handle)))
        goto exit_func;

    ret.api_version = handle->api_version;

    if (setup_gss_names(rqstp, &client_name, &service_name) < 0) {
        ret.code = KADM5_FAILURE;
        goto exit_func;
    }
    prime_arg = arg->exp;
    if (prime_arg == NULL)
        prime_arg = "*";

    if (CHANGEPW_SERVICE(rqstp) || !kadm5int_acl_check(handle->context,
                                                       rqst2name(rqstp),
                                                       ACL_LIST, NULL, NULL)) {
        ret.code = KADM5_AUTH_LIST;
        log_unauth("kadm5_get_policies", prime_arg,
                   &client_name, &service_name, rqstp);
    } else {
        ret.code  = kadm5_get_policies((void *)handle,
                                       arg->exp, &ret.pols,
                                       &ret.count);
        if( ret.code != 0 )
            errmsg = krb5_get_error_message(handle->context, ret.code);

        log_done("kadm5_get_policies", prime_arg, errmsg,
                 &client_name, &service_name, rqstp);

         if (errmsg != NULL)
             krb5_free_error_message(handle->context, errmsg);
     }
exit_func:
     gss_release_buffer(&minor_stat, &client_name);
     gss_release_buffer(&minor_stat, &service_name);
     free_server_handle(handle);
     return &ret;
 }
