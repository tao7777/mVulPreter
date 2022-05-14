static void php_mb_regex_free_cache(php_mb_regex_t **pre) 
 {
 	onig_free(*pre);
 }
