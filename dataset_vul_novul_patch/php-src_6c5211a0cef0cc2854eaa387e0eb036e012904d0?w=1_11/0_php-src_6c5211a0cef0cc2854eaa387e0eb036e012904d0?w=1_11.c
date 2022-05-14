PHP_FUNCTION(mcrypt_module_get_supported_key_sizes)
 {
 	int i, count = 0;
 	int *key_sizes;

 	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir)
 	array_init(return_value);
 
	key_sizes = mcrypt_module_get_algo_supported_key_sizes(module, dir, &count);

	for (i = 0; i < count; i++) {
		add_index_long(return_value, i, key_sizes[i]);
	}
	mcrypt_free(key_sizes);
}
