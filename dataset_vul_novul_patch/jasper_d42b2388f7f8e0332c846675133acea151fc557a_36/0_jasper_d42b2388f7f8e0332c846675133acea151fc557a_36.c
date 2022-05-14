static int jpc_ppm_putparms(jpc_ms_t *ms, jpc_cstate_t *cstate, jas_stream_t *out)
{
	jpc_ppm_t *ppm = &ms->parms.ppm;

 	/* Eliminate compiler warning about unused variables. */
 	cstate = 0;
 
	if (JAS_CAST(jas_uint, jas_stream_write(out, (char *) ppm->data, ppm->len)) != ppm->len) {
 		return -1;
 	}
 	return 0;
}
