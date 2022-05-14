static int generate(struct crypto_rng *tfm, const u8 *src, unsigned int slen,
		    u8 *dst, unsigned int dlen)
{
	return crypto_old_rng_alg(tfm)->rng_make_random(tfm, dst, dlen);
}
