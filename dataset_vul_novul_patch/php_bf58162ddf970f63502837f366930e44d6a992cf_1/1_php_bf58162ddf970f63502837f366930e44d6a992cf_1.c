PHP_METHOD(Phar, getSupportedSignatures)
{
        if (zend_parse_parameters_none() == FAILURE) {
                return;
        }
        array_init(return_value);
 
        add_next_index_stringl(return_value, "MD5", 3, 1);
	add_next_index_stringl(return_value, "SHA-1", 5, 1);
#ifdef PHAR_HASH_OK
	add_next_index_stringl(return_value, "SHA-256", 7, 1);
	add_next_index_stringl(return_value, "SHA-512", 7, 1);
#endif
#if PHAR_HAVE_OPENSSL
	add_next_index_stringl(return_value, "OpenSSL", 7, 1);
#else
	if (zend_hash_exists(&module_registry, "openssl", sizeof("openssl"))) {
		add_next_index_stringl(return_value, "OpenSSL", 7, 1);
	}
#endif
}
