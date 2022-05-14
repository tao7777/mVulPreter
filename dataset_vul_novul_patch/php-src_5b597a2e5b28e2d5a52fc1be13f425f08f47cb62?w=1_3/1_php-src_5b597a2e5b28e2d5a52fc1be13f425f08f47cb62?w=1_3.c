static void _php_mb_regex_globals_dtor(zend_mb_regex_globals *pglobals TSRMLS_DC) 
 {
 	zend_hash_destroy(&pglobals->ht_rc);
 }
