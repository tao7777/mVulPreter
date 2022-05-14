PHP_FUNCTION(mcrypt_create_iv)
{
	char *iv;
	long source = RANDOM;
	long size;
	int n = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &size, &source) == FAILURE) {
		return;
	}

	if (size <= 0 || size >= INT_MAX) {
 		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create an IV with a size of less than 1 or greater than %d", INT_MAX);
 		RETURN_FALSE;
 	}

 	iv = ecalloc(size + 1, 1);

 	if (source == RANDOM || source == URANDOM) {
 #if PHP_WIN32
 		/* random/urandom equivalent on Windows */
		BYTE *iv_b = (BYTE *) iv;
		if (php_win32_get_random_bytes(iv_b, (size_t) size) == FAILURE){
			efree(iv);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not gather sufficient random data");
			RETURN_FALSE;
		}
		n = size;
#else
		int    *fd = &MCG(fd[source]);
		size_t read_bytes = 0;

		if (*fd < 0) {
			*fd = open(source == RANDOM ? "/dev/random" : "/dev/urandom", O_RDONLY);
			if (*fd < 0) {
				efree(iv);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot open source device");
				RETURN_FALSE;
			}
		}

		while (read_bytes < size) {
			n = read(*fd, iv + read_bytes, size - read_bytes);
			if (n < 0) {
				break;
			}
			read_bytes += n;
		}
		n = read_bytes;

		if (n < size) {
			efree(iv);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not gather sufficient random data");
			RETURN_FALSE;
		}
#endif
	} else {
		n = size;
		while (size) {
			iv[--size] = (char) (255.0 * php_rand(TSRMLS_C) / RAND_MAX);
		}
	}
	RETURN_STRINGL(iv, n, 0);
}
