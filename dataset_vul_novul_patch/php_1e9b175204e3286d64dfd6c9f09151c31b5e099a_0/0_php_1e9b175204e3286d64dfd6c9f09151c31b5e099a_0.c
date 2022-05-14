PHP_METHOD(Phar, createDefaultStub)
{
	char *index = NULL, *webindex = NULL, *error;
        zend_string *stub;
        size_t index_len = 0, webindex_len = 0;
 
       if (zend_parse_parameters(ZEND_NUM_ARGS(), "|pp", &index, &index_len, &webindex, &webindex_len) == FAILURE) {
                return;
        }
 
	stub = phar_create_default_stub(index, webindex, &error);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
		efree(error);
		return;
	}
	RETURN_NEW_STR(stub);
}
