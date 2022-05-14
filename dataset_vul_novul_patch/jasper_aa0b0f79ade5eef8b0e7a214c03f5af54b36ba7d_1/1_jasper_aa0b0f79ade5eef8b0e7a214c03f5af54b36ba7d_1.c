static int jpc_pi_nextrlcp(register jpc_pi_t *pi)
{
	jpc_pchg_t *pchg;
	int *prclyrno;

	pchg = pi->pchg;
	if (!pi->prgvolfirst) {
		assert(pi->prcno < pi->pirlvl->numprcs);
		prclyrno = &pi->pirlvl->prclyrnos[pi->prcno];
		goto skip;
	} else {
		pi->prgvolfirst = 0;
	}

	for (pi->rlvlno = pchg->rlvlnostart; pi->rlvlno < pi->maxrlvls &&
	  pi->rlvlno < pchg->rlvlnoend; ++pi->rlvlno) {
		for (pi->lyrno = 0; pi->lyrno < pi->numlyrs && pi->lyrno <
 		  JAS_CAST(int, pchg->lyrnoend); ++pi->lyrno) {
 			for (pi->compno = pchg->compnostart, pi->picomp =
 			  &pi->picomps[pi->compno]; pi->compno < pi->numcomps &&
			  pi->compno < JAS_CAST(int, pchg->compnoend); ++pi->compno, ++pi->picomp) {
 				if (pi->rlvlno >= pi->picomp->numrlvls) {
 					continue;
 				}
				pi->pirlvl = &pi->picomp->pirlvls[pi->rlvlno];
				for (pi->prcno = 0, prclyrno = pi->pirlvl->prclyrnos;
				  pi->prcno < pi->pirlvl->numprcs; ++pi->prcno, ++prclyrno) {
					if (pi->lyrno >= *prclyrno) {
						*prclyrno = pi->lyrno;
						++(*prclyrno);
						return 0;
					}
skip:
					;
				}
			}
		}
	}
	return 1;
}
