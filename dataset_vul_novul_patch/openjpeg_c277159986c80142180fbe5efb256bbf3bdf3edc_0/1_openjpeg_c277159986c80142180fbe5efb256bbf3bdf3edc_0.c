 static opj_bool pi_next_lrcp(opj_pi_iterator_t * pi)
 {
     opj_pi_comp_t *comp = NULL;
    opj_pi_resolution_t *res = NULL;
    long index = 0;

    if (!pi->first) {
        comp = &pi->comps[pi->compno];
        res = &comp->resolutions[pi->resno];
        goto LABEL_SKIP;
    } else {
        pi->first = 0;
    }

    for (pi->layno = pi->poc.layno0; pi->layno < pi->poc.layno1; pi->layno++) {
        for (pi->resno = pi->poc.resno0; pi->resno < pi->poc.resno1;
                pi->resno++) {
            for (pi->compno = pi->poc.compno0; pi->compno < pi->poc.compno1; pi->compno++) {
                comp = &pi->comps[pi->compno];
                if (pi->resno >= comp->numresolutions) {
                    continue;
                }
                res = &comp->resolutions[pi->resno];
                if (!pi->tp_on) {
                    pi->poc.precno1 = res->pw * res->ph;
                }
                 for (pi->precno = pi->poc.precno0; pi->precno < pi->poc.precno1; pi->precno++) {
                     index = pi->layno * pi->step_l + pi->resno * pi->step_r + pi->compno *
                             pi->step_c + pi->precno * pi->step_p;
                     if (!pi->include[index]) {
                         pi->include[index] = 1;
                         return OPJ_TRUE;
                    }
LABEL_SKIP:
                    ;
                }
            }
        }
    }

    return OPJ_FALSE;
}
