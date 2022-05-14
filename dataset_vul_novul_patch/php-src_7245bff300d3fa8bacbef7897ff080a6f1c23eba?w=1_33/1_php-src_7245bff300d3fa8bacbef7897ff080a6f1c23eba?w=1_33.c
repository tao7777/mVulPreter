SPL_METHOD(SplFileObject, hasChildren)
{
 	if (zend_parse_parameters_none() == FAILURE) {
 		return;
 	}
 	RETURN_FALSE;
 } /* }}} */
 
/* {{{ proto bool SplFileObject::getChildren()
