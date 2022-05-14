 PHP_FUNCTION(mcrypt_module_get_algo_key_size)
 {
 	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir);
 	RETURN_LONG(mcrypt_module_get_algo_key_size(module, dir));
 }
