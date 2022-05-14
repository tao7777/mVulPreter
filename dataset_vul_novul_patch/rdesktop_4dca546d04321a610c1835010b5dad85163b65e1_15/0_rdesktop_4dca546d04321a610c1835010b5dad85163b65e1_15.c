 sec_decrypt(uint8 * data, int length)
 {
	if (length <= 0)
		return;

 	if (g_sec_decrypt_use_count == 4096)
 	{
 		sec_update(g_sec_decrypt_key, g_sec_decrypt_update_key);
		rdssl_rc4_set_key(&g_rc4_decrypt_key, g_sec_decrypt_key, g_rc4_key_len);
		g_sec_decrypt_use_count = 0;
	}

	rdssl_rc4_crypt(&g_rc4_decrypt_key, data, data, length);
	g_sec_decrypt_use_count++;
}
