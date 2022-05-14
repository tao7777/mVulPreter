 getprivs_ret * get_privs_2_svc(krb5_ui_4 *arg, struct svc_req *rqstp)
 {
     static getprivs_ret            ret;
    gss_buffer_desc                client_name, service_name;
     OM_uint32                      minor_stat;
     kadm5_server_handle_t          handle;
     const char                     *errmsg = NULL;

    xdr_free(xdr_getprivs_ret, &ret);

    if ((ret.code = new_server_handle(*arg, rqstp, &handle)))
        goto exit_func;

    if ((ret.code = check_handle((void *)handle)))
        goto exit_func;

    ret.api_version = handle->api_version;

    if (setup_gss_names(rqstp, &client_name, &service_name) < 0) {
        ret.code = KADM5_FAILURE;
        goto exit_func;
    }

    ret.code = kadm5_get_privs((void *)handle, &ret.privs);
    if( ret.code != 0 )
        errmsg = krb5_get_error_message(handle->context, ret.code);

    log_done("kadm5_get_privs", client_name.value, errmsg,
             &client_name, &service_name, rqstp);

     if (errmsg != NULL)
         krb5_free_error_message(handle->context, errmsg);
 
     gss_release_buffer(&minor_stat, &client_name);
     gss_release_buffer(&minor_stat, &service_name);
exit_func:
     free_server_handle(handle);
     return &ret;
 }
