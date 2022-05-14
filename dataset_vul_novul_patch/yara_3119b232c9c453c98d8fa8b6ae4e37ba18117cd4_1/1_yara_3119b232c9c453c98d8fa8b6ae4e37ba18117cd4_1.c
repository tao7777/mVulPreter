 int re_yyget_lineno  (yyscan_t yyscanner)
 {
     struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
         if (! YY_CURRENT_BUFFER)
             return 0;
     
    return yylineno;
}
