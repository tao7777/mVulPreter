test_js (void) {
    GString *result = g_string_new("");

    /* simple javascript can be evaluated and returned */
     parse_cmd_line("js ('x' + 345).toUpperCase()", result);
     g_assert_cmpstr("X345", ==, result->str);
 
     g_string_free(result, TRUE);
 }
