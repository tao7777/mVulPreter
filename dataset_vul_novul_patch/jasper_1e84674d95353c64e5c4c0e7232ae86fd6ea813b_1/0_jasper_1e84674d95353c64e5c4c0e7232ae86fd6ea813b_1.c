static int jpc_qcd_dumpparms(jpc_ms_t *ms, FILE *out)
{
	jpc_qcd_t *qcd = &ms->parms.qcd;
	int i;
	fprintf(out, "qntsty = %d; numguard = %d; numstepsizes = %d\n",
 	  (int) qcd->compparms.qntsty, qcd->compparms.numguard, qcd->compparms.numstepsizes);
 	for (i = 0; i < qcd->compparms.numstepsizes; ++i) {
 		fprintf(out, "expn[%d] = 0x%04x; mant[%d] = 0x%04x;\n",
		  i, JAS_CAST(unsigned, JPC_QCX_GETEXPN(qcd->compparms.stepsizes[i])),
		  i, JAS_CAST(unsigned, JPC_QCX_GETMANT(qcd->compparms.stepsizes[i])));
 	}
 	return 0;
 }
