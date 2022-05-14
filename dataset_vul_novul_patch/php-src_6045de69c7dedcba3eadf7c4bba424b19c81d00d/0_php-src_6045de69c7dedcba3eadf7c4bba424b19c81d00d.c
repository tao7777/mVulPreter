void pdo_stmt_init(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "PDOStatement", pdo_dbstmt_functions);
	pdo_dbstmt_ce = zend_register_internal_class(&ce TSRMLS_CC);
	pdo_dbstmt_ce->get_iterator = pdo_stmt_iter_get;
	pdo_dbstmt_ce->create_object = pdo_dbstmt_new;
	zend_class_implements(pdo_dbstmt_ce TSRMLS_CC, 1, zend_ce_traversable);
	zend_declare_property_null(pdo_dbstmt_ce, "queryString", sizeof("queryString")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	memcpy(&pdo_dbstmt_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	pdo_dbstmt_object_handlers.write_property = dbstmt_prop_write;
	pdo_dbstmt_object_handlers.unset_property = dbstmt_prop_delete;
	pdo_dbstmt_object_handlers.get_method = dbstmt_method_get;
	pdo_dbstmt_object_handlers.compare_objects = dbstmt_compare;
	pdo_dbstmt_object_handlers.clone_obj = dbstmt_clone_obj;

	INIT_CLASS_ENTRY(ce, "PDORow", pdo_row_functions);
	pdo_row_ce = zend_register_internal_class(&ce TSRMLS_CC);
 	pdo_row_ce->ce_flags |= ZEND_ACC_FINAL_CLASS; /* when removing this a lot of handlers need to be redone */
 	pdo_row_ce->create_object = pdo_row_new;
 	pdo_row_ce->serialize = pdo_row_serialize;
	pdo_row_ce->unserialize = zend_class_unserialize_deny;
 }
