get_policy_2_svc(gpol_arg *arg, struct svc_req *rqstp)
{
     static gpol_ret             ret;
     kadm5_ret_t         ret2;
     char                        *prime_arg, *funcname;
    gss_buffer_desc             client_name = GSS_C_EMPTY_BUFFER;
    gss_buffer_desc             service_name = GSS_C_EMPTY_BUFFER;
     OM_uint32                   minor_stat;
     kadm5_principal_ent_rec     caller_ent;
     kadm5_server_handle_t       handle;
    const char                  *errmsg = NULL;

    xdr_free(xdr_gpol_ret,  &ret);

    if ((ret.code = new_server_handle(arg->api_version, rqstp, &handle)))
        goto exit_func;

    if ((ret.code = check_handle((void *)handle)))
        goto exit_func;

    ret.api_version = handle->api_version;

    funcname = "kadm5_get_policy";

    if (setup_gss_names(rqstp, &client_name, &service_name) < 0) {
        ret.code = KADM5_FAILURE;
        goto exit_func;
    }
    prime_arg = arg->name;

    ret.code = KADM5_AUTH_GET;
    if (!CHANGEPW_SERVICE(rqstp) && kadm5int_acl_check(handle->context,
                                                       rqst2name(rqstp),
                                                       ACL_INQUIRE, NULL, NULL))
        ret.code = KADM5_OK;
    else {
        ret.code = kadm5_get_principal(handle->lhandle,
                                       handle->current_caller,
                                       &caller_ent,
                                       KADM5_PRINCIPAL_NORMAL_MASK);
        if (ret.code == KADM5_OK) {
            if (caller_ent.aux_attributes & KADM5_POLICY &&
                strcmp(caller_ent.policy, arg->name) == 0) {
                ret.code = KADM5_OK;
            } else ret.code = KADM5_AUTH_GET;
            ret2 = kadm5_free_principal_ent(handle->lhandle,
                                            &caller_ent);
            ret.code = ret.code ? ret.code : ret2;
        }
    }

    if (ret.code == KADM5_OK) {
        ret.code = kadm5_get_policy(handle, arg->name, &ret.rec);

        if( ret.code != 0 )
            errmsg = krb5_get_error_message(handle->context, ret.code);

        log_done(funcname,
                 ((prime_arg == NULL) ? "(null)" : prime_arg), errmsg,
                 &client_name, &service_name, rqstp);
        if (errmsg != NULL)
            krb5_free_error_message(handle->context, errmsg);

    } else {
         log_unauth(funcname, prime_arg,
                    &client_name, &service_name, rqstp);
     }
exit_func:
     gss_release_buffer(&minor_stat, &client_name);
     gss_release_buffer(&minor_stat, &service_name);
     free_server_handle(handle);
     return &ret;
 
}
