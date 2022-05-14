check_rpcsec_auth(struct svc_req *rqstp)
{
     gss_ctx_id_t ctx;
     krb5_context kctx;
     OM_uint32 maj_stat, min_stat;
     gss_name_t name;
     krb5_principal princ;
     int ret, success;
     krb5_data *c1, *c2, *realm;
     gss_buffer_desc gss_str;
     kadm5_server_handle_t handle;
     size_t slen;
     char *sdots;

     success = 0;
     handle = (kadm5_server_handle_t)global_server_handle;

     if (rqstp->rq_cred.oa_flavor != RPCSEC_GSS)
	  return 0;

     ctx = rqstp->rq_svccred;

     maj_stat = gss_inquire_context(&min_stat, ctx, NULL, &name,
				    NULL, NULL, NULL, NULL, NULL);
     if (maj_stat != GSS_S_COMPLETE) {
	  krb5_klog_syslog(LOG_ERR, _("check_rpcsec_auth: failed "
				      "inquire_context, stat=%u"), maj_stat);
	  log_badauth(maj_stat, min_stat, rqstp->rq_xprt, NULL);
	  goto fail_name;
     }

     kctx = handle->context;
     ret = gss_to_krb5_name_1(rqstp, kctx, name, &princ, &gss_str);
     if (ret == 0)
	  goto fail_name;

     slen = gss_str.length;
     trunc_name(&slen, &sdots);
     /*
      * Since we accept with GSS_C_NO_NAME, the client can authenticate
      * against the entire kdb.  Therefore, ensure that the service
      * name is something reasonable.
      */
     if (krb5_princ_size(kctx, princ) != 2)
	  goto fail_princ;

      c1 = krb5_princ_component(kctx, princ, 0);
      c2 = krb5_princ_component(kctx, princ, 1);
      realm = krb5_princ_realm(kctx, princ);
     success = data_eq_string(*realm, handle->params.realm) &&
	     data_eq_string(*c1, "kadmin") && !data_eq_string(*c2, "history");
 
 fail_princ:
      if (!success) {
	 krb5_klog_syslog(LOG_ERR, _("bad service principal %.*s%s"),
			  (int) slen, (char *) gss_str.value, sdots);
     }
     gss_release_buffer(&min_stat, &gss_str);
     krb5_free_principal(kctx, princ);
fail_name:
     gss_release_name(&min_stat, &name);
     return success;
}
