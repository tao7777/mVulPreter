decrypt_response(struct sc_card *card, unsigned char *in, size_t inlen, unsigned char *out, size_t * out_len)
{
	size_t cipher_len;
	size_t i;
	unsigned char iv[16] = { 0 };
	unsigned char plaintext[4096] = { 0 };
	epass2003_exdata *exdata = NULL;

	if (!card->drv_data) 
		return SC_ERROR_INVALID_ARGUMENTS;

	exdata = (epass2003_exdata *)card->drv_data;

	/* no cipher */
	if (in[0] == 0x99)
		return 0;

	/* parse cipher length */
	if (0x01 == in[2] && 0x82 != in[1]) {
		cipher_len = in[1];
		i = 3;
	}
	else if (0x01 == in[3] && 0x81 == in[1]) {
		cipher_len = in[2];
		i = 4;
	}
	else if (0x01 == in[4] && 0x82 == in[1]) {
		cipher_len = in[2] * 0x100;
		cipher_len += in[3];
		i = 5;
	}
	else {
		return -1;
	}

	if (cipher_len < 2 || i+cipher_len > inlen || cipher_len > sizeof plaintext)
		return -1;

	/* decrypt */
	if (KEY_TYPE_AES == exdata->smtype)
		aes128_decrypt_cbc(exdata->sk_enc, 16, iv, &in[i], cipher_len - 1, plaintext);
	else
		des3_decrypt_cbc(exdata->sk_enc, 16, iv, &in[i], cipher_len - 1, plaintext);

	/* unpadding */
 	while (0x80 != plaintext[cipher_len - 2] && (cipher_len - 2 > 0))
 		cipher_len--;
 
	if (2 == cipher_len || *out_len < cipher_len - 2)
 		return -1;
 
 	memcpy(out, plaintext, cipher_len - 2);
	*out_len = cipher_len - 2;
	return 0;
}
