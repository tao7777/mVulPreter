PHP_FUNCTION(mcrypt_enc_get_supported_key_sizes)
 {
 	int i, count = 0;
 	int *key_sizes;
 	MCRYPT_GET_TD_ARG
 	array_init(return_value);
 
	key_sizes = mcrypt_enc_get_supported_key_sizes(pm->td, &count);

	for (i = 0; i < count; i++) {
		add_index_long(return_value, i, key_sizes[i]);
	}

	mcrypt_free(key_sizes);
}
