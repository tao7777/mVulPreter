spnego_gss_pseudo_random(OM_uint32 *minor_status,
			 gss_ctx_id_t context,
			 int prf_key,
			 const gss_buffer_t prf_in,
			 ssize_t desired_output_len,
 			 gss_buffer_t prf_out)
 {
 	OM_uint32 ret;
 	ret = gss_pseudo_random(minor_status,
				context,
 				prf_key,
 				prf_in,
 				desired_output_len,
				prf_out);
        return (ret);
}
