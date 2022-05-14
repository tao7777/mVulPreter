int re_yylex_init_extra(YY_EXTRA_TYPE yy_user_defined,yyscan_t* ptr_yy_globals )

{
    struct yyguts_t dummy_yyguts;

    re_yyset_extra (yy_user_defined, &dummy_yyguts);

    if (ptr_yy_globals == NULL){
         errno = EINVAL;
         return 1;
     }
     *ptr_yy_globals = (yyscan_t) re_yyalloc ( sizeof( struct yyguts_t ), &dummy_yyguts );
     if (*ptr_yy_globals == NULL){
         errno = ENOMEM;
         return 1;
     }
     /* By setting to 0xAA, we expose bugs in
     yy_init_globals. Leave at 0x00 for releases. */
     memset(*ptr_yy_globals,0x00,sizeof(struct yyguts_t));
     re_yyset_extra (yy_user_defined, *ptr_yy_globals);
     return yy_init_globals ( *ptr_yy_globals );
 }
