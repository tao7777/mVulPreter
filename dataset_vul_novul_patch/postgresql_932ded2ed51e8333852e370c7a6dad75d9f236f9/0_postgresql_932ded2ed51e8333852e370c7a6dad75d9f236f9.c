px_crypt_des(const char *key, const char *setting)
{
	int			i;
	uint32		count,
				salt,
				l,
				r0,
				r1,
				keybuf[2];
	char	   *p;
	uint8	   *q;
	static char output[21];

	if (!des_initialised)
		des_init();


	/*
	 * Copy the key, shifting each character up by one bit and padding with
	 * zeros.
	 */
    q = (uint8 *) keybuf;
    while (q - (uint8 *) keybuf - 8)
    {
       *q++ = *key << 1;
       if (*key != '\0')
            key++;
    }
    if (des_setkey((char *) keybuf))

#ifndef DISABLE_XDES
	if (*setting == _PASSWORD_EFMT1)
	{
		/*
		 * "new"-style: setting - underscore, 4 bytes of count, 4 bytes of
		 * salt key - unlimited characters
		 */
		for (i = 1, count = 0L; i < 5; i++)
			count |= ascii_to_bin(setting[i]) << (i - 1) * 6;

		for (i = 5, salt = 0L; i < 9; i++)
			salt |= ascii_to_bin(setting[i]) << (i - 5) * 6;

		while (*key)
		{
			/*
			 * Encrypt the key with itself.
			 */
			if (des_cipher((char *) keybuf, (char *) keybuf, 0L, 1))
				return (NULL);

			/*
			 * And XOR with the next 8 characters of the key.
			 */
			q = (uint8 *) keybuf;
			while (q - (uint8 *) keybuf - 8 && *key)
				*q++ ^= *key++ << 1;

			if (des_setkey((char *) keybuf))
				return (NULL);
		}
		strncpy(output, setting, 9);

		/*
		 * Double check that we weren't given a short setting. If we were, the
		 * above code will probably have created weird values for count and
		 * salt, but we don't really care. Just make sure the output string
		 * doesn't have an extra NUL in it.
		 */
		output[9] = '\0';
		p = output + strlen(output);
	}
	else
#endif   /* !DISABLE_XDES */
	{
		/*
		 * "old"-style: setting - 2 bytes of salt key - up to 8 characters
		 */
		count = 25;

		salt = (ascii_to_bin(setting[1]) << 6)
			| ascii_to_bin(setting[0]);

		output[0] = setting[0];

		/*
		 * If the encrypted password that the salt was extracted from is only
		 * 1 character long, the salt will be corrupted.  We need to ensure
		 * that the output string doesn't have an extra NUL in it!
		 */
		output[1] = setting[1] ? setting[1] : output[0];

		p = output + 2;
	}
	setup_salt(salt);

	/*
	 * Do it.
	 */
	if (do_des(0L, 0L, &r0, &r1, count))
		return (NULL);

	/*
	 * Now encode the result...
	 */
	l = (r0 >> 8);
	*p++ = _crypt_a64[(l >> 18) & 0x3f];
	*p++ = _crypt_a64[(l >> 12) & 0x3f];
	*p++ = _crypt_a64[(l >> 6) & 0x3f];
	*p++ = _crypt_a64[l & 0x3f];

	l = (r0 << 16) | ((r1 >> 16) & 0xffff);
	*p++ = _crypt_a64[(l >> 18) & 0x3f];
	*p++ = _crypt_a64[(l >> 12) & 0x3f];
	*p++ = _crypt_a64[(l >> 6) & 0x3f];
	*p++ = _crypt_a64[l & 0x3f];

	l = r1 << 2;
	*p++ = _crypt_a64[(l >> 12) & 0x3f];
	*p++ = _crypt_a64[(l >> 6) & 0x3f];
	*p++ = _crypt_a64[l & 0x3f];
	*p = 0;

	return (output);
}
