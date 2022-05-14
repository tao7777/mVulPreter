int RAND_DRBG_generate(RAND_DRBG *drbg, unsigned char *out, size_t outlen,
                        int prediction_resistance,
                        const unsigned char *adin, size_t adinlen)
 {
     int reseed_required = 0;
 
     if (drbg->state != DRBG_READY) {
        rand_drbg_restart(drbg, NULL, 0, 0);

        if (drbg->state == DRBG_ERROR) {
            RANDerr(RAND_F_RAND_DRBG_GENERATE, RAND_R_IN_ERROR_STATE);
            return 0;
        }
        if (drbg->state == DRBG_UNINITIALISED) {
            RANDerr(RAND_F_RAND_DRBG_GENERATE, RAND_R_NOT_INSTANTIATED);
            return 0;
        }
    }

    if (outlen > drbg->max_request) {
        RANDerr(RAND_F_RAND_DRBG_GENERATE, RAND_R_REQUEST_TOO_LARGE_FOR_DRBG);
        return 0;
    }
    if (adinlen > drbg->max_adinlen) {
        RANDerr(RAND_F_RAND_DRBG_GENERATE, RAND_R_ADDITIONAL_INPUT_TOO_LONG);
        return 0;
         return 0;
     }
 
    if (drbg->fork_count != rand_fork_count) {
        drbg->fork_count = rand_fork_count;
         reseed_required = 1;
     }
    }
