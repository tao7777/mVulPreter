SPL_METHOD(SplObjectStorage, unserialize)
{
	spl_SplObjectStorage *intern = Z_SPLOBJSTORAGE_P(getThis());

	char *buf;
	size_t buf_len;
	const unsigned char *p, *s;
	php_unserialize_data_t var_hash;
	zval entry, inf;
	zval *pcount, *pmembers;
	spl_SplObjectStorageElement *element;
	zend_long count;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &buf, &buf_len) == FAILURE) {
		return;
	}

	if (buf_len == 0) {
		return;
	}

	/* storage */
	s = p = (const unsigned char*)buf;
	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	if (*p!= 'x' || *++p != ':') {
		goto outexcept;
	}
	++p;

	pcount = var_tmp_var(&var_hash);
	if (!php_var_unserialize(pcount, &p, s + buf_len, &var_hash) || Z_TYPE_P(pcount) != IS_LONG) {
		goto outexcept;
	}

 	--p; /* for ';' */
 	count = Z_LVAL_P(pcount);
 
	ZVAL_UNDEF(&entry);
	ZVAL_UNDEF(&inf);

 	while (count-- > 0) {
 		spl_SplObjectStorageElement *pelement;
 		zend_string *hash;

		if (*p != ';') {
			goto outexcept;
		}
		++p;
		if(*p != 'O' && *p != 'C' && *p != 'r') {
			goto outexcept;
		}
		/* store reference to allow cross-references between different elements */
 		if (!php_var_unserialize(&entry, &p, s + buf_len, &var_hash)) {
 			goto outexcept;
 		}
 		if (*p == ',') { /* new version has inf */
 			++p;
 			if (!php_var_unserialize(&inf, &p, s + buf_len, &var_hash)) {
 				zval_ptr_dtor(&entry);
 				goto outexcept;
 			}
		}
		if (Z_TYPE(entry) != IS_OBJECT) {
			zval_ptr_dtor(&entry);
			zval_ptr_dtor(&inf);
			goto outexcept;
 		}
 
 		hash = spl_object_storage_get_hash(intern, getThis(), &entry);
		if (!hash) {
			zval_ptr_dtor(&entry);
			zval_ptr_dtor(&inf);
			goto outexcept;
		}
		pelement = spl_object_storage_get(intern, hash);
		spl_object_storage_free_hash(intern, hash);
		if (pelement) {
			if (!Z_ISUNDEF(pelement->inf)) {
				var_push_dtor(&var_hash, &pelement->inf);
			}
			if (!Z_ISUNDEF(pelement->obj)) {
				var_push_dtor(&var_hash, &pelement->obj);
			}
		}
		element = spl_object_storage_attach(intern, getThis(), &entry, Z_ISUNDEF(inf)?NULL:&inf);
		var_replace(&var_hash, &entry, &element->obj);
		var_replace(&var_hash, &inf, &element->inf);
		zval_ptr_dtor(&entry);
		ZVAL_UNDEF(&entry);
		zval_ptr_dtor(&inf);
		ZVAL_UNDEF(&inf);
	}

	if (*p != ';') {
		goto outexcept;
	}
	++p;

	/* members */
	if (*p!= 'm' || *++p != ':') {
		goto outexcept;
	}
	++p;

	pmembers = var_tmp_var(&var_hash);
	if (!php_var_unserialize(pmembers, &p, s + buf_len, &var_hash) || Z_TYPE_P(pmembers) != IS_ARRAY) {
		goto outexcept;
	}

	/* copy members */
	object_properties_load(&intern->std, Z_ARRVAL_P(pmembers));

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	return;

outexcept:
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "Error at offset %pd of %d bytes", (zend_long)((char*)p - buf), buf_len);
	return;

} /* }}} */

ZEND_BEGIN_ARG_INFO(arginfo_Object, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_attach, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, inf)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_Serialized, 0)
	ZEND_ARG_INFO(0, serialized)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_setInfo, 0)
	ZEND_ARG_INFO(0, info)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_getHash, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_offsetGet, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_splobject_void, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry spl_funcs_SplObjectStorage[] = {
	SPL_ME(SplObjectStorage,  attach,      arginfo_attach,        0)
	SPL_ME(SplObjectStorage,  detach,      arginfo_Object,        0)
	SPL_ME(SplObjectStorage,  contains,    arginfo_Object,        0)
	SPL_ME(SplObjectStorage,  addAll,      arginfo_Object,        0)
	SPL_ME(SplObjectStorage,  removeAll,   arginfo_Object,        0)
	SPL_ME(SplObjectStorage,  removeAllExcept,   arginfo_Object,  0)
	SPL_ME(SplObjectStorage,  getInfo,     arginfo_splobject_void,0)
	SPL_ME(SplObjectStorage,  setInfo,     arginfo_setInfo,       0)
	SPL_ME(SplObjectStorage,  getHash,     arginfo_getHash,       0)
	/* Countable */
	SPL_ME(SplObjectStorage,  count,       arginfo_splobject_void,0)
	/* Iterator */
	SPL_ME(SplObjectStorage,  rewind,      arginfo_splobject_void,0)
	SPL_ME(SplObjectStorage,  valid,       arginfo_splobject_void,0)
	SPL_ME(SplObjectStorage,  key,         arginfo_splobject_void,0)
	SPL_ME(SplObjectStorage,  current,     arginfo_splobject_void,0)
	SPL_ME(SplObjectStorage,  next,        arginfo_splobject_void,0)
	/* Serializable */
	SPL_ME(SplObjectStorage,  unserialize, arginfo_Serialized,    0)
	SPL_ME(SplObjectStorage,  serialize,   arginfo_splobject_void,0)
	/* ArrayAccess */
	SPL_MA(SplObjectStorage, offsetExists, SplObjectStorage, contains, arginfo_offsetGet, 0)
	SPL_MA(SplObjectStorage, offsetSet,    SplObjectStorage, attach,   arginfo_attach, 0)
	SPL_MA(SplObjectStorage, offsetUnset,  SplObjectStorage, detach,   arginfo_offsetGet, 0)
	SPL_ME(SplObjectStorage, offsetGet,    arginfo_offsetGet,     0)
	{NULL, NULL, NULL}
};

typedef enum {
	MIT_NEED_ANY     = 0,
	MIT_NEED_ALL     = 1,
	MIT_KEYS_NUMERIC = 0,
	MIT_KEYS_ASSOC   = 2
} MultipleIteratorFlags;

#define SPL_MULTIPLE_ITERATOR_GET_ALL_CURRENT   1
#define SPL_MULTIPLE_ITERATOR_GET_ALL_KEY       2

/* {{{ proto void MultipleIterator::__construct([int flags = MIT_NEED_ALL|MIT_KEYS_NUMERIC])
