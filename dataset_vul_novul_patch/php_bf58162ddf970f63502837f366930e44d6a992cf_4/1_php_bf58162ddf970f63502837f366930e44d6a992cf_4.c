 PHP_METHOD(Phar, count)
 {
        PHAR_ARCHIVE_OBJECT();
        if (zend_parse_parameters_none() == FAILURE) {
                return;
        }

	RETURN_LONG(zend_hash_num_elements(&phar_obj->arc.archive->manifest));
}
