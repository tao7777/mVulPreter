kdc_process_s4u_x509_user(krb5_context context,
                          krb5_kdc_req *request,
                          krb5_pa_data *pa_data,
                          krb5_keyblock *tgs_subkey,
                          krb5_keyblock *tgs_session,
                          krb5_pa_s4u_x509_user **s4u_x509_user,
                          const char **status)
{
    krb5_error_code             code;
    krb5_data                   req_data;

    req_data.length = pa_data->length;
     req_data.data = (char *)pa_data->contents;
 
     code = decode_krb5_pa_s4u_x509_user(&req_data, s4u_x509_user);
    if (code)
         return code;
 
     code = verify_s4u_x509_user_checksum(context,
                                          tgs_subkey ? tgs_subkey :
                                         tgs_session,
                                         &req_data,
                                         request->nonce, *s4u_x509_user);

    if (code) {
        *status = "INVALID_S4U2SELF_CHECKSUM";
        krb5_free_pa_s4u_x509_user(context, *s4u_x509_user);
        *s4u_x509_user = NULL;
        return code;
    }

    if (krb5_princ_size(context, (*s4u_x509_user)->user_id.user) == 0 ||
        (*s4u_x509_user)->user_id.subject_cert.length != 0) {
        *status = "INVALID_S4U2SELF_REQUEST";
        krb5_free_pa_s4u_x509_user(context, *s4u_x509_user);
        *s4u_x509_user = NULL;
        return KRB5KDC_ERR_C_PRINCIPAL_UNKNOWN;
    }

    return 0;
}
