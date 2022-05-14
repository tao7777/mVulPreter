BOOL rdp_decrypt(rdpRdp* rdp, STREAM* s, int length, UINT16 securityFlags)
{
	BYTE cmac[8];
	BYTE wmac[8];

	if (rdp->settings->EncryptionMethods == ENCRYPTION_METHOD_FIPS)
	{
		UINT16 len;
		BYTE version, pad;
		BYTE* sig;

		if (stream_get_left(s) < 12)
			return FALSE;

		stream_read_UINT16(s, len); /* 0x10 */
		stream_read_BYTE(s, version); /* 0x1 */
		stream_read_BYTE(s, pad);

		sig = s->p;
		stream_seek(s, 8);	/* signature */

		length -= 12;

		if (!security_fips_decrypt(s->p, length, rdp))
		{
			printf("FATAL: cannot decrypt\n");
			return FALSE; /* TODO */
		}

		if (!security_fips_check_signature(s->p, length - pad, sig, rdp))
		{
			printf("FATAL: invalid packet signature\n");
			return FALSE; /* TODO */
		}

		/* is this what needs adjusting? */
		s->size -= pad;
		return TRUE;
	}

	if (stream_get_left(s) < 8)
		return FALSE;
 
 	stream_read(s, wmac, sizeof(wmac));
 	length -= sizeof(wmac);
	if (!security_decrypt(s->p, length, rdp))
		return FALSE;
 
 	if (securityFlags & SEC_SECURE_CHECKSUM)
 		security_salted_mac_signature(rdp, s->p, length, FALSE, cmac);
	else
		security_mac_signature(rdp, s->p, length, cmac);

	if (memcmp(wmac, cmac, sizeof(wmac)) != 0)
	{
		printf("WARNING: invalid packet signature\n");
		/*
		 * Because Standard RDP Security is totally broken,
		 * and cannot protect against MITM, don't treat signature
		 * verification failure as critical. This at least enables
		 * us to work with broken RDP clients and servers that
		 * generate invalid signatures.
		 */
	}

	return TRUE;
}
