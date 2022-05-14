PHP_METHOD(Phar, isValidPharFilename)
{
	char *fname;
	const char *ext_str;
	size_t fname_len;
        int ext_len, is_executable;
        zend_bool executable = 1;
 
       if (zend_parse_parameters(ZEND_NUM_ARGS(), "p|b", &fname, &fname_len, &executable) == FAILURE) {
                return;
        }
 
	is_executable = executable;
	RETVAL_BOOL(phar_detect_phar_fname_ext(fname, fname_len, &ext_str, &ext_len, is_executable, 2, 1) == SUCCESS);
}
