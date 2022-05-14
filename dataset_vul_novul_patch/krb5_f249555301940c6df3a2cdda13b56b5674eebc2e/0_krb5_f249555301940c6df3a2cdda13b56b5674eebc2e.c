pkinit_check_kdc_pkid(krb5_context context,
                      pkinit_plg_crypto_context plg_cryptoctx,
                      pkinit_req_crypto_context req_cryptoctx,
                      pkinit_identity_crypto_context id_cryptoctx,
                      unsigned char *pdid_buf,
                      unsigned int pkid_len,
                      int *valid_kdcPkId)
{
    krb5_error_code retval = KRB5KDC_ERR_PREAUTH_FAILED;
    PKCS7_ISSUER_AND_SERIAL *is = NULL;
    const unsigned char *p = pdid_buf;
    int status = 1;
    X509 *kdc_cert = sk_X509_value(id_cryptoctx->my_certs, id_cryptoctx->cert_index);

    *valid_kdcPkId = 0;
     pkiDebug("found kdcPkId in AS REQ\n");
     is = d2i_PKCS7_ISSUER_AND_SERIAL(NULL, &p, (int)pkid_len);
     if (is == NULL)
        return retval;
 
     status = X509_NAME_cmp(X509_get_issuer_name(kdc_cert), is->issuer);
     if (!status) {
        status = ASN1_INTEGER_cmp(X509_get_serialNumber(kdc_cert), is->serial);
        if (!status)
            *valid_kdcPkId = 1;
     }
 
     retval = 0;
     X509_NAME_free(is->issuer);
     ASN1_INTEGER_free(is->serial);
     free(is);

    return retval;
}
