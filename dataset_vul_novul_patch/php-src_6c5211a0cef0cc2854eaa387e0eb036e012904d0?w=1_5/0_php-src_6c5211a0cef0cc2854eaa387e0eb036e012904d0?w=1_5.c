 PHP_FUNCTION(mcrypt_module_self_test)
 {
 	MCRYPT_GET_MODE_DIR_ARGS(algorithms_dir);

 	if (mcrypt_module_self_test(module, dir) == 0) {
 		RETURN_TRUE;
 	} else {
		RETURN_FALSE;
	}
}
