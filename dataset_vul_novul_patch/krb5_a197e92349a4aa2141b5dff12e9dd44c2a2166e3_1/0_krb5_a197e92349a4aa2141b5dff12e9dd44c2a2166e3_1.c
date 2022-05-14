bool_t auth_gssapi_unwrap_data(
     OM_uint32 *major,
     OM_uint32 *minor,
     gss_ctx_id_t context,
     uint32_t seq_num,
     XDR *in_xdrs,
     bool_t (*xdr_func)(),
     caddr_t xdr_ptr)
{
     gss_buffer_desc in_buf, out_buf;
     XDR temp_xdrs;
     uint32_t verf_seq_num;
     int conf, qop;
     unsigned int length;

     PRINTF(("gssapi_unwrap_data: starting\n"));

     *major = GSS_S_COMPLETE;
     *minor = 0; /* assumption */

     in_buf.value = NULL;
     out_buf.value = NULL;
     if (! xdr_bytes(in_xdrs, (char **) &in_buf.value,
		     &length, (unsigned int) -1)) {
	 PRINTF(("gssapi_unwrap_data: deserializing encrypted data failed\n"));
	 temp_xdrs.x_op = XDR_FREE;
	 (void)xdr_bytes(&temp_xdrs, (char **) &in_buf.value, &length,
			 (unsigned int) -1);
	 return FALSE;
     }
     in_buf.length = length;

     *major = gss_unseal(minor, context, &in_buf, &out_buf, &conf,
			 &qop);
     free(in_buf.value);
     if (*major != GSS_S_COMPLETE)
	  return FALSE;

     PRINTF(("gssapi_unwrap_data: %llu bytes data, %llu bytes sealed\n",
	     (unsigned long long)out_buf.length,
	     (unsigned long long)in_buf.length));

     xdrmem_create(&temp_xdrs, out_buf.value, out_buf.length, XDR_DECODE);

     /* deserialize the sequence number */
     if (! xdr_u_int32(&temp_xdrs, &verf_seq_num)) {
	  PRINTF(("gssapi_unwrap_data: deserializing verf_seq_num failed\n"));
	  gss_release_buffer(minor, &out_buf);
	  XDR_DESTROY(&temp_xdrs);
	  return FALSE;
     }
     if (verf_seq_num != seq_num) {
	  PRINTF(("gssapi_unwrap_data: seq %d specified, read %d\n",
		  seq_num, verf_seq_num));
	  gss_release_buffer(minor, &out_buf);
	  XDR_DESTROY(&temp_xdrs);
	  return FALSE;
     }
     PRINTF(("gssapi_unwrap_data: unwrap seq_num %d okay\n", verf_seq_num));

     /* deserialize the arguments into xdr_ptr */
      if (! (*xdr_func)(&temp_xdrs, xdr_ptr)) {
 	  PRINTF(("gssapi_unwrap_data: deserializing arguments failed\n"));
 	  gss_release_buffer(minor, &out_buf);
 	  XDR_DESTROY(&temp_xdrs);
 	  return FALSE;
      }

     PRINTF(("gssapi_unwrap_data: succeeding\n\n"));

     gss_release_buffer(minor, &out_buf);
     XDR_DESTROY(&temp_xdrs);
     return TRUE;
}
