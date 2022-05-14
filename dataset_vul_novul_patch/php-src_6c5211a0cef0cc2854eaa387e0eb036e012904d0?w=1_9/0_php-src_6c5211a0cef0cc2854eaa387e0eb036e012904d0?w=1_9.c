 PHP_FUNCTION(mcrypt_module_get_algo_block_size)
 {
 	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir)

 	RETURN_LONG(mcrypt_module_get_algo_block_size(module, dir));
 }
