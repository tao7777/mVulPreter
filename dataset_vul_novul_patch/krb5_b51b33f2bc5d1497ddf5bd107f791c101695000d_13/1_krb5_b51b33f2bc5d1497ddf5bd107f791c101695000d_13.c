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
 
	ret = gss_inquire_context(minor_status,
				context_handle,
				src_name,
				targ_name,
				lifetime_rec,
				mech_type,
				ctx_flags,
				locally_initiated,
				opened);
 
 	return (ret);
 }
