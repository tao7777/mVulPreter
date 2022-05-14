static int rngapi_reset(struct crypto_rng *tfm, const u8 *seed,
			unsigned int slen)
{
	u8 *buf = NULL;
	u8 *src = (u8 *)seed;
	int err;
	if (slen) {
		buf = kmalloc(slen, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;
		memcpy(buf, seed, slen);
		src = buf;
	}
	err = crypto_old_rng_alg(tfm)->rng_reset(tfm, src, slen);
	kzfree(buf);
	return err;
}
