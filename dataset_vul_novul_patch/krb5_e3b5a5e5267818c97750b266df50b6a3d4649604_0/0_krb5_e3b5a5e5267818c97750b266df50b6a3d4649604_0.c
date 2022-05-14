otp_verify(krb5_context context, krb5_data *req_pkt, krb5_kdc_req *request,
           krb5_enc_tkt_part *enc_tkt_reply, krb5_pa_data *pa,
           krb5_kdcpreauth_callbacks cb, krb5_kdcpreauth_rock rock,
           krb5_kdcpreauth_moddata moddata,
           krb5_kdcpreauth_verify_respond_fn respond, void *arg)
{
    krb5_keyblock *armor_key = NULL;
    krb5_pa_otp_req *req = NULL;
    struct request_state *rs;
    krb5_error_code retval;
     krb5_data d, plaintext;
     char *config;
 
     /* Get the FAST armor key. */
     armor_key = cb->fast_armor(context, rock);
     if (armor_key == NULL) {
        retval = KRB5KDC_ERR_PREAUTH_FAILED;
        com_err("otp", retval, "No armor key found when verifying padata");
        goto error;
    }

    /* Decode the request. */
    d = make_data(pa->contents, pa->length);
    retval = decode_krb5_pa_otp_req(&d, &req);
    if (retval != 0) {
        com_err("otp", retval, "Unable to decode OTP request");
        goto error;
    }

    /* Decrypt the nonce from the request. */
    retval = decrypt_encdata(context, armor_key, req, &plaintext);
    if (retval != 0) {
        com_err("otp", retval, "Unable to decrypt nonce");
        goto error;
    }

    /* Verify the nonce or timestamp. */
    retval = nonce_verify(context, armor_key, &plaintext);
    if (retval != 0)
        retval = timestamp_verify(context, &plaintext);
    krb5_free_data_contents(context, &plaintext);
    if (retval != 0) {
        com_err("otp", retval, "Unable to verify nonce or timestamp");
         goto error;
     }
 
    /* Create the request state.  Save the response callback, and the
     * enc_tkt_reply pointer so we can set the TKT_FLG_PRE_AUTH flag later. */
     rs = k5alloc(sizeof(struct request_state), &retval);
     if (rs == NULL)
         goto error;
     rs->arg = arg;
     rs->respond = respond;
    rs->enc_tkt_reply = enc_tkt_reply;
 
     /* Get the principal's OTP configuration string. */
     retval = cb->get_string(context, rock, "otp", &config);
    if (retval == 0 && config == NULL)
        retval = KRB5_PREAUTH_FAILED;
    if (retval != 0) {
        free(rs);
        goto error;
    }

    /* Send the request. */
    otp_state_verify((otp_state *)moddata, cb->event_context(context, rock),
                     request->client, config, req, on_response, rs);
    cb->free_string(context, rock, config);

    k5_free_pa_otp_req(context, req);
    return;

error:
    k5_free_pa_otp_req(context, req);
    (*respond)(arg, retval, NULL, NULL, NULL);
}
