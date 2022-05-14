 PHP_FUNCTION(mcrypt_module_is_block_algorithm)
 {
 	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir)

 	if (mcrypt_module_is_block_algorithm(module, dir) == 1) {
 		RETURN_TRUE;
 	} else {
		RETURN_FALSE;
	}
}
