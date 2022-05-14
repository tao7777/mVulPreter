decrypt_response(struct sc_card *card, unsigned char *in, unsigned char *out, size_t * out_len)
 {
	size_t in_len;
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
		in_len = in[1];
 		i = 3;
 	}
 	else if (0x01 == in[3] && 0x81 == in[1]) {
		in_len = in[2];
 		i = 4;
 	}
 	else if (0x01 == in[4] && 0x82 == in[1]) {
		in_len = in[2] * 0x100;
		in_len += in[3];
 		i = 5;
 	}
 	else {
 		return -1;
 	}
 
 	/* decrypt */
 	if (KEY_TYPE_AES == exdata->smtype)
		aes128_decrypt_cbc(exdata->sk_enc, 16, iv, &in[i], in_len - 1, plaintext);
 	else
		des3_decrypt_cbc(exdata->sk_enc, 16, iv, &in[i], in_len - 1, plaintext);
 
 	/* unpadding */
	while (0x80 != plaintext[in_len - 2] && (in_len - 2 > 0))
		in_len--;
 
	if (2 == in_len)
 		return -1;
 
	memcpy(out, plaintext, in_len - 2);
	*out_len = in_len - 2;
 	return 0;
 }
