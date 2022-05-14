 BOOL security_decrypt(BYTE* data, int length, rdpRdp* rdp)
 {
	if (rdp->rc4_decrypt_key == NULL)
		return FALSE;
 	if (rdp->decrypt_use_count >= 4096)
 	{
 		security_key_update(rdp->decrypt_key, rdp->decrypt_update_key, rdp->rc4_key_len);
		crypto_rc4_free(rdp->rc4_decrypt_key);
		rdp->rc4_decrypt_key = crypto_rc4_init(rdp->decrypt_key, rdp->rc4_key_len);
		rdp->decrypt_use_count = 0;
	}
	crypto_rc4(rdp->rc4_decrypt_key, length, data, data);
	rdp->decrypt_use_count += 1;
	rdp->decrypt_checksum_use_count++;
	return TRUE;
}
