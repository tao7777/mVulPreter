err_t verify_signed_hash(const struct RSA_public_key *k
                         , u_char *s, unsigned int s_max_octets
                         , u_char **psig
                         , size_t hash_len
                         , const u_char *sig_val, size_t sig_len)
{
    unsigned int padlen;

    /* actual exponentiation; see PKCS#1 v2.0 5.1 */
    {
	chunk_t temp_s;
	MP_INT c;

	n_to_mpz(&c, sig_val, sig_len);
	oswcrypto.mod_exp(&c, &c, &k->e, &k->n);

	temp_s = mpz_to_n(&c, sig_len);	/* back to octets */
        if(s_max_octets < sig_len) {
            return "2""exponentiation failed; too many octets";
        }
	memcpy(s, temp_s.ptr, sig_len);
	pfree(temp_s.ptr);
	mpz_clear(&c);
    }

    /* check signature contents */
    /* verify padding (not including any DER digest info! */
    padlen = sig_len - 3 - hash_len;
    /* now check padding */

    DBG(DBG_CRYPT,
	DBG_dump("verify_sh decrypted SIG1:", s, sig_len));
    DBG(DBG_CRYPT, DBG_log("pad_len calculated: %d hash_len: %d", padlen, (int)hash_len));

    /* skip padding */
    if(s[0]    != 0x00
       || s[1] != 0x01
       || s[padlen+2] != 0x00) {
 	return "3""SIG padding does not check out";
     }
 
    /* signature starts after ASN wrapped padding [00,01,FF..FF,00] */
    (*psig) = s + padlen + 3;

    /* verify padding contents */
    {
        const u_char *p;
        size_t cnt_ffs = 0;

        for (p = s+2; p < s+padlen+2; p++)
            if (*p == 0xFF)
                cnt_ffs ++;

        if (cnt_ffs != padlen)
            return "4" "invalid Padding String";
    }
 
     /* return SUCCESS */
     return NULL;
}
