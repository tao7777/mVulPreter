gstd_accept(int fd, char **display_creds, char **export_name, char **mech)
{
	gss_name_t	 client;
	gss_OID		 mech_oid;
	struct gstd_tok *tok;
	gss_ctx_id_t	 ctx = GSS_C_NO_CONTEXT;
	gss_buffer_desc	 in, out;
	OM_uint32	 maj, min;
	int		 ret;

	*display_creds = NULL;
	*export_name = NULL;
	out.length = 0;
	in.length = 0;
	read_packet(fd, &in, 60000, 1);
again:
	while ((ret = read_packet(fd, &in, 60000, 0)) == -2)
		;

	if (ret < 1)
		return NULL;

	maj = gss_accept_sec_context(&min, &ctx, GSS_C_NO_CREDENTIAL,
 	    &in, GSS_C_NO_CHANNEL_BINDINGS, &client, &mech_oid, &out, NULL,
 	    NULL, NULL);
 
	gss_release_buffer(&min, &in);

 	if (out.length && write_packet(fd, &out)) {
 		gss_release_buffer(&min, &out);
 		return NULL;
 	}
	gss_release_buffer(&min, &out);
 
 	GSTD_GSS_ERROR(maj, min, NULL, "gss_accept_sec_context");
 
	if (maj & GSS_S_CONTINUE_NEEDED)
		goto again;

	*display_creds = gstd_get_display_name(client);
	*export_name = gstd_get_export_name(client);
	*mech = gstd_get_mech(mech_oid);

	gss_release_name(&min, &client);
	SETUP_GSTD_TOK(tok, ctx, fd, "gstd_accept");
	return tok;
}
