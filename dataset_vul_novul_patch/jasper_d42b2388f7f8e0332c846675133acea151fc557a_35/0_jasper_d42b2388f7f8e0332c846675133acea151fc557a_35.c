static int jpc_ppm_getparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *in)
{
	jpc_ppm_t *ppm = &ms->parms.ppm;

	/* Eliminate compiler warning about unused variables. */
	cstate = 0;

	ppm->data = 0;

	if (ms->len < 1) {
		goto error;
	}
	if (jpc_getuint8(in, &ppm->ind)) {
		goto error;
	}

	ppm->len = ms->len - 1;
	if (ppm->len > 0) {
 		if (!(ppm->data = jas_malloc(ppm->len))) {
 			goto error;
 		}
		if (JAS_CAST(jas_uint, jas_stream_read(in, ppm->data, ppm->len)) != ppm->len) {
 			goto error;
 		}
 	} else {
		ppm->data = 0;
	}
	return 0;

error:
	jpc_ppm_destroyparms(ms);
	return -1;
}
