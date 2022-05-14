PHP_FUNCTION(locale_get_display_language) 
 {
     get_icu_disp_value_src_php( LOC_LANG_TAG , INTERNAL_FUNCTION_PARAM_PASSTHRU );
 }
