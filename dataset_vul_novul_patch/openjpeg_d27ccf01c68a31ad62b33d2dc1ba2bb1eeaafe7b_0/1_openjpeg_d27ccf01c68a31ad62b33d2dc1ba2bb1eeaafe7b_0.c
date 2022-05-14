static OPJ_BOOL opj_pi_next_pcrl(opj_pi_iterator_t * pi)
{
    opj_pi_comp_t *comp = NULL;
    opj_pi_resolution_t *res = NULL;
    OPJ_UINT32 index = 0;

    if (!pi->first) {
        comp = &pi->comps[pi->compno];
        goto LABEL_SKIP;
    } else {
        OPJ_UINT32 compno, resno;
        pi->first = 0;
        pi->dx = 0;
        pi->dy = 0;
        for (compno = 0; compno < pi->numcomps; compno++) {
            comp = &pi->comps[compno];
            for (resno = 0; resno < comp->numresolutions; resno++) {
                OPJ_UINT32 dx, dy;
                res = &comp->resolutions[resno];
                dx = comp->dx * (1u << (res->pdx + comp->numresolutions - 1 - resno));
                dy = comp->dy * (1u << (res->pdy + comp->numresolutions - 1 - resno));
                pi->dx = !pi->dx ? dx : opj_uint_min(pi->dx, dx);
                pi->dy = !pi->dy ? dy : opj_uint_min(pi->dy, dy);
            }
        }
    }
    if (!pi->tp_on) {
        pi->poc.ty0 = pi->ty0;
        pi->poc.tx0 = pi->tx0;
        pi->poc.ty1 = pi->ty1;
        pi->poc.tx1 = pi->tx1;
    }
    for (pi->y = pi->poc.ty0; pi->y < pi->poc.ty1;
            pi->y += (OPJ_INT32)(pi->dy - (OPJ_UINT32)(pi->y % (OPJ_INT32)pi->dy))) {
        for (pi->x = pi->poc.tx0; pi->x < pi->poc.tx1;
                pi->x += (OPJ_INT32)(pi->dx - (OPJ_UINT32)(pi->x % (OPJ_INT32)pi->dx))) {
            for (pi->compno = pi->poc.compno0; pi->compno < pi->poc.compno1; pi->compno++) {
                comp = &pi->comps[pi->compno];
                for (pi->resno = pi->poc.resno0;
                        pi->resno < opj_uint_min(pi->poc.resno1, comp->numresolutions); pi->resno++) {
                    OPJ_UINT32 levelno;
                    OPJ_INT32 trx0, try0;
                    OPJ_INT32 trx1, try1;
                    OPJ_UINT32 rpx, rpy;
                    OPJ_INT32 prci, prcj;
                    res = &comp->resolutions[pi->resno];
                    levelno = comp->numresolutions - 1 - pi->resno;
                    trx0 = opj_int_ceildiv(pi->tx0, (OPJ_INT32)(comp->dx << levelno));
                    try0 = opj_int_ceildiv(pi->ty0, (OPJ_INT32)(comp->dy << levelno));
                    trx1 = opj_int_ceildiv(pi->tx1, (OPJ_INT32)(comp->dx << levelno));
                     try1 = opj_int_ceildiv(pi->ty1, (OPJ_INT32)(comp->dy << levelno));
                     rpx = res->pdx + levelno;
                     rpy = res->pdy + levelno;
                     if (!((pi->y % (OPJ_INT32)(comp->dy << rpy) == 0) || ((pi->y == pi->ty0) &&
                             ((try0 << levelno) % (1 << rpy))))) {
                         continue;
                    }
                    if (!((pi->x % (OPJ_INT32)(comp->dx << rpx) == 0) || ((pi->x == pi->tx0) &&
                            ((trx0 << levelno) % (1 << rpx))))) {
                        continue;
                    }

                    if ((res->pw == 0) || (res->ph == 0)) {
                        continue;
                    }

                    if ((trx0 == trx1) || (try0 == try1)) {
                        continue;
                    }

                    prci = opj_int_floordivpow2(opj_int_ceildiv(pi->x,
                                                (OPJ_INT32)(comp->dx << levelno)), (OPJ_INT32)res->pdx)
                           - opj_int_floordivpow2(trx0, (OPJ_INT32)res->pdx);
                    prcj = opj_int_floordivpow2(opj_int_ceildiv(pi->y,
                                                (OPJ_INT32)(comp->dy << levelno)), (OPJ_INT32)res->pdy)
                           - opj_int_floordivpow2(try0, (OPJ_INT32)res->pdy);
                    pi->precno = (OPJ_UINT32)(prci + prcj * (OPJ_INT32)res->pw);
                    for (pi->layno = pi->poc.layno0; pi->layno < pi->poc.layno1; pi->layno++) {
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
    }

    return OPJ_FALSE;
}
