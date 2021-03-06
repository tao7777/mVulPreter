 int DH_check_pub_key(const DH *dh, const BIGNUM *pub_key, int *ret)
 {
     int ok = 0;
    BIGNUM *q = NULL;
 
     *ret = 0;
    q = BN_new();
    if (q == NULL)
         goto err;
    BN_set_word(q, 1);
    if (BN_cmp(pub_key, q) <= 0)
         *ret |= DH_CHECK_PUBKEY_TOO_SMALL;
    BN_copy(q, dh->p);
    BN_sub_word(q, 1);
    if (BN_cmp(pub_key, q) >= 0)
         *ret |= DH_CHECK_PUBKEY_TOO_LARGE;
 
     ok = 1;
  err:
    if (q != NULL)
        BN_free(q);
     return (ok);
 }
