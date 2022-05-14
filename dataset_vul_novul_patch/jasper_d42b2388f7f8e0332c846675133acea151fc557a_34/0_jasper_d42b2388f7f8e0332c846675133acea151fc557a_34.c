jpc_ms_t *jpc_getms(jas_stream_t *in, jpc_cstate_t *cstate)
{
	jpc_ms_t *ms;
	jpc_mstabent_t *mstabent;
	jas_stream_t *tmpstream;

	if (!(ms = jpc_ms_create(0))) {
		return 0;
	}

	/* Get the marker type. */
	if (jpc_getuint16(in, &ms->id) || ms->id < JPC_MS_MIN ||
	  ms->id > JPC_MS_MAX) {
		jpc_ms_destroy(ms);
		return 0;
	}

	mstabent = jpc_mstab_lookup(ms->id);
	ms->ops = &mstabent->ops;

	/* Get the marker segment length and parameters if present. */
	/* Note: It is tacitly assumed that a marker segment cannot have
	  parameters unless it has a length field.  That is, there cannot
	  be a parameters field without a length field and vice versa. */
	if (JPC_MS_HASPARMS(ms->id)) {
		/* Get the length of the marker segment. */
		if (jpc_getuint16(in, &ms->len) || ms->len < 3) {
			jpc_ms_destroy(ms);
			return 0;
		}
		/* Calculate the length of the marker segment parameters. */
		ms->len -= 2;
		/* Create and prepare a temporary memory stream from which to
		  read the marker segment parameters. */
		/* Note: This approach provides a simple way of ensuring that
		  we never read beyond the end of the marker segment (even if
		  the marker segment length is errantly set too small). */
		if (!(tmpstream = jas_stream_memopen(0, 0))) {
			jpc_ms_destroy(ms);
			return 0;
		}
		if (jas_stream_copy(tmpstream, in, ms->len) ||
		  jas_stream_seek(tmpstream, 0, SEEK_SET) < 0) {
			jas_stream_close(tmpstream);
			jpc_ms_destroy(ms);
			return 0;
		}
		/* Get the marker segment parameters. */
		if ((*ms->ops->getparms)(ms, cstate, tmpstream)) {
			ms->ops = 0;
			jpc_ms_destroy(ms);
			jas_stream_close(tmpstream);
			return 0;
		}

		if (jas_getdbglevel() > 0) {
 			jpc_ms_dump(ms, stderr);
 		}
 
		if (JAS_CAST(jas_ulong, jas_stream_tell(tmpstream)) != ms->len) {
 			jas_eprintf(
 			  "warning: trailing garbage in marker segment (%ld bytes)\n",
 			  ms->len - jas_stream_tell(tmpstream));
		}

		/* Close the temporary stream. */
		jas_stream_close(tmpstream);

	} else {
		/* There are no marker segment parameters. */
		ms->len = 0;

		if (jas_getdbglevel() > 0) {
			jpc_ms_dump(ms, stderr);
		}
	}

	/* Update the code stream state information based on the type of
	  marker segment read. */
	/* Note: This is a bit of a hack, but I'm not going to define another
	  type of virtual function for this one special case. */
	if (ms->id == JPC_MS_SIZ) {
		cstate->numcomps = ms->parms.siz.numcomps;
	}

	return ms;
}
