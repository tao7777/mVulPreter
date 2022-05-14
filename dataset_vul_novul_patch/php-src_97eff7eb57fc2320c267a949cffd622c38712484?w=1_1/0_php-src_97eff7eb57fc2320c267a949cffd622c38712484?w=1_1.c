PHP_FUNCTION(locale_get_primary_language ) 
PHP_FUNCTION(locale_get_primary_language )
 {
 	get_icu_value_src_php( LOC_LANG_TAG , INTERNAL_FUNCTION_PARAM_PASSTHRU );
 }
