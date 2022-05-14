chrand_principal_2_svc(chrand_arg *arg, struct svc_req *rqstp)
{
    static chrand_ret           ret;
     krb5_keyblock               *k;
     int                         nkeys;
     char                        *prime_arg, *funcname;
    gss_buffer_desc             client_name = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc             service_name = GSS_C_EMPTY_BUFFER;
     OM_uint32                   minor_stat;
     kadm5_server_handle_t       handle;
     const char                  *errmsg = NULL;

    xdr_free(xdr_chrand_ret, &ret);

    if ((ret.code = new_server_handle(arg->api_version, rqstp, &handle)))
        goto exit_func;


    if ((ret.code = check_handle((void *)handle)))
        goto exit_func;

    ret.api_version = handle->api_version;

    funcname = "kadm5_randkey_principal";

    if (setup_gss_names(rqstp, &client_name, &service_name) < 0) {
        ret.code = KADM5_FAILURE;
        goto exit_func;
    }
    if (krb5_unparse_name(handle->context, arg->princ, &prime_arg)) {
        ret.code = KADM5_BAD_PRINCIPAL;
        goto exit_func;
    }

    if (cmp_gss_krb5_name(handle, rqst2name(rqstp), arg->princ)) {
        ret.code = randkey_principal_wrapper_3((void *)handle, arg->princ,
                                               FALSE, 0, NULL, &k, &nkeys);
    } else if (!(CHANGEPW_SERVICE(rqstp)) &&
               kadm5int_acl_check(handle->context, rqst2name(rqstp),
                                  ACL_CHANGEPW, arg->princ, NULL)) {
        ret.code = kadm5_randkey_principal((void *)handle, arg->princ,
                                           &k, &nkeys);
    } else {
        log_unauth(funcname, prime_arg,
                   &client_name, &service_name, rqstp);
        ret.code = KADM5_AUTH_CHANGEPW;
    }

    if(ret.code == KADM5_OK) {
        ret.keys = k;
        ret.n_keys = nkeys;
    }

    if(ret.code != KADM5_AUTH_CHANGEPW) {
        if( ret.code != 0 )
            errmsg = krb5_get_error_message(handle->context, ret.code);

        log_done(funcname, prime_arg, errmsg,
                 &client_name, &service_name, rqstp);

        if (errmsg != NULL)
             krb5_free_error_message(handle->context, errmsg);
     }
     free(prime_arg);
exit_func:
     gss_release_buffer(&minor_stat, &client_name);
     gss_release_buffer(&minor_stat, &service_name);
     free_server_handle(handle);
     return &ret;
 }
