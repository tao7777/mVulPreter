spnego_gss_inquire_context(
			OM_uint32	*minor_status,
			const gss_ctx_id_t context_handle,
			gss_name_t	*src_name,
			gss_name_t	*targ_name,
			OM_uint32	*lifetime_rec,
			gss_OID		*mech_type,
			OM_uint32	*ctx_flags,
			int		*locally_initiated,
 			int		*opened)
 {
 	OM_uint32 ret = GSS_S_COMPLETE;
	spnego_gss_ctx_id_t sc = (spnego_gss_ctx_id_t)context_handle;

	if (src_name != NULL)
		*src_name = GSS_C_NO_NAME;
	if (targ_name != NULL)
		*targ_name = GSS_C_NO_NAME;
	if (lifetime_rec != NULL)
		*lifetime_rec = 0;
	if (mech_type != NULL)
		*mech_type = (gss_OID)gss_mech_spnego;
	if (ctx_flags != NULL)
		*ctx_flags = 0;
	if (locally_initiated != NULL)
		*locally_initiated = sc->initiate;
	if (opened != NULL)
		*opened = sc->opened;

	if (sc->ctx_handle != GSS_C_NO_CONTEXT) {
		ret = gss_inquire_context(minor_status, sc->ctx_handle,
					  src_name, targ_name, lifetime_rec,
					  mech_type, ctx_flags, NULL, NULL);
	}
 
	if (!sc->opened) {
		/*
		 * We are still doing SPNEGO negotiation, so report SPNEGO as
		 * the OID.  After negotiation is complete we will report the
		 * underlying mechanism OID.
		 */
		if (mech_type != NULL)
			*mech_type = (gss_OID)gss_mech_spnego;

		/*
		 * Remove flags we don't support with partially-established
		 * contexts.  (Change this to keep GSS_C_TRANS_FLAG if we add
		 * support for exporting partial SPNEGO contexts.)
		 */
		if (ctx_flags != NULL) {
			*ctx_flags &= ~GSS_C_PROT_READY_FLAG;
			*ctx_flags &= ~GSS_C_TRANS_FLAG;
		}
	}
 
 	return (ret);
 }
