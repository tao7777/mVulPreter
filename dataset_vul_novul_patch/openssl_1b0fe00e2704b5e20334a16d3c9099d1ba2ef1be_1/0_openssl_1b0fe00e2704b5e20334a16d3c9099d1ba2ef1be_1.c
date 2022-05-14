static RAND_DRBG *rand_drbg_new(int secure,
                                int type,
                                unsigned int flags,
                                RAND_DRBG *parent)
{
    RAND_DRBG *drbg = secure ? OPENSSL_secure_zalloc(sizeof(*drbg))
                             : OPENSSL_zalloc(sizeof(*drbg));

    if (drbg == NULL) {
        RANDerr(RAND_F_RAND_DRBG_NEW, ERR_R_MALLOC_FAILURE);
        return NULL;
     }
 
     drbg->secure = secure && CRYPTO_secure_allocated(drbg);
    drbg->fork_id = openssl_get_fork_id();
     drbg->parent = parent;
 
     if (parent == NULL) {
        drbg->get_entropy = rand_drbg_get_entropy;
        drbg->cleanup_entropy = rand_drbg_cleanup_entropy;
#ifndef RAND_DRBG_GET_RANDOM_NONCE
        drbg->get_nonce = rand_drbg_get_nonce;
        drbg->cleanup_nonce = rand_drbg_cleanup_nonce;
#endif

        drbg->reseed_interval = master_reseed_interval;
        drbg->reseed_time_interval = master_reseed_time_interval;
    } else {
        drbg->get_entropy = rand_drbg_get_entropy;
        drbg->cleanup_entropy = rand_drbg_cleanup_entropy;
        /*
         * Do not provide nonce callbacks, the child DRBGs will
         * obtain their nonce using random bits from the parent.
         */

        drbg->reseed_interval = slave_reseed_interval;
        drbg->reseed_time_interval = slave_reseed_time_interval;
    }

    if (RAND_DRBG_set(drbg, type, flags) == 0)
        goto err;

    if (parent != NULL) {
        rand_drbg_lock(parent);
        if (drbg->strength > parent->strength) {
            /*
             * We currently don't support the algorithm from NIST SP 800-90C
             * 10.1.2 to use a weaker DRBG as source
             */
            rand_drbg_unlock(parent);
            RANDerr(RAND_F_RAND_DRBG_NEW, RAND_R_PARENT_STRENGTH_TOO_WEAK);
            goto err;
        }
        rand_drbg_unlock(parent);
    }

    return drbg;

 err:
    RAND_DRBG_free(drbg);

    return NULL;
}
