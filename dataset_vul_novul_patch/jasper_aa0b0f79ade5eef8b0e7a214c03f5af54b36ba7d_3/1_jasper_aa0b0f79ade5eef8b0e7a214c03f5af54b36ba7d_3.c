int jpc_dec_decodepkts(jpc_dec_t *dec, jas_stream_t *pkthdrstream, jas_stream_t *in)
{
	jpc_dec_tile_t *tile;
	jpc_pi_t *pi;
	int ret;

 	tile = dec->curtile;
 	pi = tile->pi;
 	for (;;) {
if (!tile->pkthdrstream || jas_stream_peekc(tile->pkthdrstream) == EOF) {
		switch (jpc_dec_lookahead(in)) {
		case JPC_MS_EOC:
		case JPC_MS_SOT:
			return 0;
			break;
		case JPC_MS_SOP:
		case JPC_MS_EPH:
		case 0:
			break;
		default:
			return -1;
			break;
 		}
}
 		if ((ret = jpc_pi_next(pi))) {
 			return ret;
 		}
if (dec->maxpkts >= 0 && dec->numpkts >= dec->maxpkts) {
	jas_eprintf("warning: stopping decode prematurely as requested\n");
	return 0;
}
 		if (jas_getdbglevel() >= 1) {
 			jas_eprintf("packet offset=%08ld prg=%d cmptno=%02d "
 			  "rlvlno=%02d prcno=%03d lyrno=%02d\n", (long)
 			  jas_stream_getrwcount(in), jpc_pi_prg(pi), jpc_pi_cmptno(pi),
 			  jpc_pi_rlvlno(pi), jpc_pi_prcno(pi), jpc_pi_lyrno(pi));
 		}
		if (jpc_dec_decodepkt(dec, pkthdrstream, in, jpc_pi_cmptno(pi), jpc_pi_rlvlno(pi),
		  jpc_pi_prcno(pi), jpc_pi_lyrno(pi))) {
 			return -1;
 		}
++dec->numpkts;
 	}
 
 	return 0;
}
