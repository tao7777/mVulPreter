static int php_session_destroy(TSRMLS_D) /* {{{ */
{
	int retval = SUCCESS;

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Trying to destroy uninitialized session");
		return FAILURE;
	}
                return FAILURE;
        }
 
       if (PS(mod)->s_destroy(&PS(mod_data), PS(id) TSRMLS_CC) == FAILURE) {
                retval = FAILURE;
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Session object destruction failed");
        }
	php_rinit_session_globals(TSRMLS_C);

	return retval;
}
/* }}} */

PHPAPI void php_add_session_var(char *name, size_t namelen TSRMLS_DC) /* {{{ */
{
	zval **sym_track = NULL;

	IF_SESSION_VARS() {
		zend_hash_find(Z_ARRVAL_P(PS(http_session_vars)), name, namelen + 1, (void *) &sym_track);
	} else {
		return;
	}

	if (sym_track == NULL) {
		zval *empty_var;

		ALLOC_INIT_ZVAL(empty_var);
		ZEND_SET_SYMBOL_WITH_LENGTH(Z_ARRVAL_P(PS(http_session_vars)), name, namelen+1, empty_var, 1, 0);
	}
}
/* }}} */

PHPAPI void php_set_session_var(char *name, size_t namelen, zval *state_val, php_unserialize_data_t *var_hash TSRMLS_DC) /* {{{ */
{
	IF_SESSION_VARS() {
		zend_set_hash_symbol(state_val, name, namelen, PZVAL_IS_REF(state_val), 1, Z_ARRVAL_P(PS(http_session_vars)));
	}
}
/* }}} */

PHPAPI int php_get_session_var(char *name, size_t namelen, zval ***state_var TSRMLS_DC) /* {{{ */
{
	int ret = FAILURE;

	IF_SESSION_VARS() {
		ret = zend_hash_find(Z_ARRVAL_P(PS(http_session_vars)), name, namelen + 1, (void **) state_var);
	}
	return ret;
}
/* }}} */

static void php_session_track_init(TSRMLS_D) /* {{{ */
{
	zval *session_vars = NULL;

	/* Unconditionally destroy existing array -- possible dirty data */
	zend_delete_global_variable("_SESSION", sizeof("_SESSION")-1 TSRMLS_CC);

	if (PS(http_session_vars)) {
		zval_ptr_dtor(&PS(http_session_vars));
	}

	MAKE_STD_ZVAL(session_vars);
	array_init(session_vars);
	PS(http_session_vars) = session_vars;

	ZEND_SET_GLOBAL_VAR_WITH_LENGTH("_SESSION", sizeof("_SESSION"), PS(http_session_vars), 2, 1);
}
/* }}} */

static char *php_session_encode(int *newlen TSRMLS_DC) /* {{{ */
{
	char *ret = NULL;

	IF_SESSION_VARS() {
		if (!PS(serializer)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown session.serialize_handler. Failed to encode session object");
			ret = NULL;
		} else if (PS(serializer)->encode(&ret, newlen TSRMLS_CC) == FAILURE) {
			ret = NULL;
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot encode non-existent session");
	}
	return ret;
}
/* }}} */

static void php_session_decode(const char *val, int vallen TSRMLS_DC) /* {{{ */
{
	if (!PS(serializer)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown session.serialize_handler. Failed to decode session object");
		return;
	}
	if (PS(serializer)->decode(val, vallen TSRMLS_CC) == FAILURE) {
		php_session_destroy(TSRMLS_C);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to decode session object. Session has been destroyed");
	}
}
/* }}} */

/*
 * Note that we cannot use the BASE64 alphabet here, because
 * it contains "/" and "+": both are unacceptable for simple inclusion
 * into URLs.
 */

static char hexconvtab[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,-";

enum {
