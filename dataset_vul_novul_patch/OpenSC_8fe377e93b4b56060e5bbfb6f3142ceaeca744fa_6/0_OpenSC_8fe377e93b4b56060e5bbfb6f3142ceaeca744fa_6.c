construct_mac_tlv(struct sc_card *card, unsigned char *apdu_buf, size_t data_tlv_len, size_t le_tlv_len,
		unsigned char *mac_tlv, size_t * mac_tlv_len, const unsigned char key_type)
{
	size_t block_size = (KEY_TYPE_AES == key_type ? 16 : 8);
	unsigned char mac[4096] = { 0 };
	size_t mac_len;
	unsigned char icv[16] = { 0 };
	int i = (KEY_TYPE_AES == key_type ? 15 : 7);
	epass2003_exdata *exdata = NULL;

	if (!card->drv_data) 
		return SC_ERROR_INVALID_ARGUMENTS;

	exdata = (epass2003_exdata *)card->drv_data;

	if (0 == data_tlv_len && 0 == le_tlv_len) {
		mac_len = block_size;
	}
	else {
		/* padding */
		*(apdu_buf + block_size + data_tlv_len + le_tlv_len) = 0x80;
		if ((data_tlv_len + le_tlv_len + 1) % block_size)
			mac_len = (((data_tlv_len + le_tlv_len + 1) / block_size) +
					1) * block_size + block_size;

		else
			mac_len = data_tlv_len + le_tlv_len + 1 + block_size;

		memset((apdu_buf + block_size + data_tlv_len + le_tlv_len + 1),
		       0, (mac_len - (data_tlv_len + le_tlv_len + 1)));
	}

	/* increase icv */
	for (; i >= 0; i--) {
		if (exdata->icv_mac[i] == 0xff) {
			exdata->icv_mac[i] = 0;
		}
		else {
			exdata->icv_mac[i]++;
			break;
		}
	}

	/* calculate MAC */
	memset(icv, 0, sizeof(icv));
	memcpy(icv, exdata->icv_mac, 16);
	if (KEY_TYPE_AES == key_type) {
		aes128_encrypt_cbc(exdata->sk_mac, 16, icv, apdu_buf, mac_len, mac);
 		memcpy(mac_tlv + 2, &mac[mac_len - 16], 8);
 	}
 	else {
		unsigned char iv[EVP_MAX_IV_LENGTH] = { 0 };
 		unsigned char tmp[8] = { 0 };
 		des_encrypt_cbc(exdata->sk_mac, 8, icv, apdu_buf, mac_len, mac);
 		des_decrypt_cbc(&exdata->sk_mac[8], 8, iv, &mac[mac_len - 8], 8, tmp);
		memset(iv, 0x00, sizeof iv);
 		des_encrypt_cbc(exdata->sk_mac, 8, iv, tmp, 8, mac_tlv + 2);
 	}
 
	*mac_tlv_len = 2 + 8;
	return 0;
}
