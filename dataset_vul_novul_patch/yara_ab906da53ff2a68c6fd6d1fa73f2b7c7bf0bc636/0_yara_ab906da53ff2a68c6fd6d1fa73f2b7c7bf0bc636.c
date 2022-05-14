yyparse (void *yyscanner, YR_COMPILER* compiler)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, yyscanner, compiler);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 8:
#line 230 "grammar.y" /* yacc.c:1646  */
    {
        int result = yr_parser_reduce_import(yyscanner, (yyvsp[0].sized_string));

        yr_free((yyvsp[0].sized_string));

        ERROR_IF(result != ERROR_SUCCESS);
      }
#line 1661 "grammar.c" /* yacc.c:1646  */
    break;

  case 9:
#line 242 "grammar.y" /* yacc.c:1646  */
    {
        YR_RULE* rule = yr_parser_reduce_rule_declaration_phase_1(
            yyscanner, (int32_t) (yyvsp[-2].integer), (yyvsp[0].c_string));

        ERROR_IF(rule == NULL);

        (yyval.rule) = rule;
      }
#line 1674 "grammar.c" /* yacc.c:1646  */
    break;

  case 10:
#line 251 "grammar.y" /* yacc.c:1646  */
    {
        YR_RULE* rule = (yyvsp[-4].rule); // rule created in phase 1

        rule->tags = (yyvsp[-3].c_string);
        rule->metas = (yyvsp[-1].meta);
        rule->strings = (yyvsp[0].string);
      }
#line 1686 "grammar.c" /* yacc.c:1646  */
    break;

  case 11:
#line 259 "grammar.y" /* yacc.c:1646  */
    {
        YR_RULE* rule = (yyvsp[-7].rule); // rule created in phase 1

        compiler->last_result = yr_parser_reduce_rule_declaration_phase_2(
            yyscanner, rule);

        yr_free((yyvsp[-8].c_string));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
      }
#line 1701 "grammar.c" /* yacc.c:1646  */
    break;

  case 12:
#line 274 "grammar.y" /* yacc.c:1646  */
    {
        (yyval.meta) = NULL;
      }
#line 1709 "grammar.c" /* yacc.c:1646  */
    break;

  case 13:
#line 278 "grammar.y" /* yacc.c:1646  */
    {

        YR_META null_meta;

        memset(&null_meta, 0xFF, sizeof(YR_META));
        null_meta.type = META_TYPE_NULL;

        compiler->last_result = yr_arena_write_data(
            compiler->metas_arena,
            &null_meta,
            sizeof(YR_META),
            NULL);

        (yyval.meta) = (yyvsp[0].meta);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
      }
#line 1736 "grammar.c" /* yacc.c:1646  */
    break;

  case 14:
#line 305 "grammar.y" /* yacc.c:1646  */
    {
        (yyval.string) = NULL;
      }
#line 1744 "grammar.c" /* yacc.c:1646  */
    break;

  case 15:
#line 309 "grammar.y" /* yacc.c:1646  */
    {

        YR_STRING null_string;

        memset(&null_string, 0xFF, sizeof(YR_STRING));
        null_string.g_flags = STRING_GFLAGS_NULL;

        compiler->last_result = yr_arena_write_data(
            compiler->strings_arena,
            &null_string,
            sizeof(YR_STRING),
            NULL);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        (yyval.string) = (yyvsp[0].string);
      }
#line 1771 "grammar.c" /* yacc.c:1646  */
    break;

  case 17:
#line 340 "grammar.y" /* yacc.c:1646  */
    { (yyval.integer) = 0;  }
#line 1777 "grammar.c" /* yacc.c:1646  */
    break;

  case 18:
#line 341 "grammar.y" /* yacc.c:1646  */
    { (yyval.integer) = (yyvsp[-1].integer) | (yyvsp[0].integer); }
#line 1783 "grammar.c" /* yacc.c:1646  */
    break;

  case 19:
#line 346 "grammar.y" /* yacc.c:1646  */
    { (yyval.integer) = RULE_GFLAGS_PRIVATE; }
#line 1789 "grammar.c" /* yacc.c:1646  */
    break;

  case 20:
#line 347 "grammar.y" /* yacc.c:1646  */
    { (yyval.integer) = RULE_GFLAGS_GLOBAL; }
#line 1795 "grammar.c" /* yacc.c:1646  */
    break;

  case 21:
#line 353 "grammar.y" /* yacc.c:1646  */
    {
        (yyval.c_string) = NULL;
      }
#line 1803 "grammar.c" /* yacc.c:1646  */
    break;

  case 22:
#line 357 "grammar.y" /* yacc.c:1646  */
    {

        compiler->last_result = yr_arena_write_string(
            yyget_extra(yyscanner)->sz_arena, "", NULL);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        (yyval.c_string) = (yyvsp[0].c_string);
      }
#line 1821 "grammar.c" /* yacc.c:1646  */
    break;

  case 23:
#line 375 "grammar.y" /* yacc.c:1646  */
    {
        char* identifier;

        compiler->last_result = yr_arena_write_string(
            yyget_extra(yyscanner)->sz_arena, (yyvsp[0].c_string), &identifier);

        yr_free((yyvsp[0].c_string));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        (yyval.c_string) = identifier;
      }
#line 1838 "grammar.c" /* yacc.c:1646  */
    break;

  case 24:
#line 388 "grammar.y" /* yacc.c:1646  */
    {
        char* tag_name = (yyvsp[-1].c_string);
        size_t tag_length = tag_name != NULL ? strlen(tag_name) : 0;

        while (tag_length > 0)
        {
          if (strcmp(tag_name, (yyvsp[0].c_string)) == 0)
          {
            yr_compiler_set_error_extra_info(compiler, tag_name);
            compiler->last_result = ERROR_DUPLICATED_TAG_IDENTIFIER;
            break;
          }

          tag_name = (char*) yr_arena_next_address(
              yyget_extra(yyscanner)->sz_arena,
              tag_name,
              tag_length + 1);

          tag_length = tag_name != NULL ? strlen(tag_name) : 0;
        }

        if (compiler->last_result == ERROR_SUCCESS)
          compiler->last_result = yr_arena_write_string(
              yyget_extra(yyscanner)->sz_arena, (yyvsp[0].c_string), NULL);

        yr_free((yyvsp[0].c_string));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        (yyval.c_string) = (yyvsp[-1].c_string);
      }
#line 1874 "grammar.c" /* yacc.c:1646  */
    break;

  case 25:
#line 424 "grammar.y" /* yacc.c:1646  */
    {  (yyval.meta) = (yyvsp[0].meta); }
#line 1880 "grammar.c" /* yacc.c:1646  */
    break;

  case 26:
#line 425 "grammar.y" /* yacc.c:1646  */
    {  (yyval.meta) = (yyvsp[-1].meta); }
#line 1886 "grammar.c" /* yacc.c:1646  */
    break;

  case 27:
#line 431 "grammar.y" /* yacc.c:1646  */
    {
        SIZED_STRING* sized_string = (yyvsp[0].sized_string);

        (yyval.meta) = yr_parser_reduce_meta_declaration(
            yyscanner,
            META_TYPE_STRING,
            (yyvsp[-2].c_string),
            sized_string->c_string,
            0);

        yr_free((yyvsp[-2].c_string));
        yr_free((yyvsp[0].sized_string));

        ERROR_IF((yyval.meta) == NULL);
      }
#line 1906 "grammar.c" /* yacc.c:1646  */
    break;

  case 28:
#line 447 "grammar.y" /* yacc.c:1646  */
    {
        (yyval.meta) = yr_parser_reduce_meta_declaration(
            yyscanner,
            META_TYPE_INTEGER,
            (yyvsp[-2].c_string),
            NULL,
            (yyvsp[0].integer));

        yr_free((yyvsp[-2].c_string));

        ERROR_IF((yyval.meta) == NULL);
      }
#line 1923 "grammar.c" /* yacc.c:1646  */
    break;

  case 29:
#line 460 "grammar.y" /* yacc.c:1646  */
    {
        (yyval.meta) = yr_parser_reduce_meta_declaration(
            yyscanner,
            META_TYPE_INTEGER,
            (yyvsp[-3].c_string),
            NULL,
            -(yyvsp[0].integer));

        yr_free((yyvsp[-3].c_string));

        ERROR_IF((yyval.meta) == NULL);
      }
#line 1940 "grammar.c" /* yacc.c:1646  */
    break;

  case 30:
#line 473 "grammar.y" /* yacc.c:1646  */
    {
        (yyval.meta) = yr_parser_reduce_meta_declaration(
            yyscanner,
            META_TYPE_BOOLEAN,
            (yyvsp[-2].c_string),
            NULL,
            TRUE);

        yr_free((yyvsp[-2].c_string));

        ERROR_IF((yyval.meta) == NULL);
      }
#line 1957 "grammar.c" /* yacc.c:1646  */
    break;

  case 31:
#line 486 "grammar.y" /* yacc.c:1646  */
    {
        (yyval.meta) = yr_parser_reduce_meta_declaration(
            yyscanner,
            META_TYPE_BOOLEAN,
            (yyvsp[-2].c_string),
            NULL,
            FALSE);

        yr_free((yyvsp[-2].c_string));

        ERROR_IF((yyval.meta) == NULL);
      }
#line 1974 "grammar.c" /* yacc.c:1646  */
    break;

  case 32:
#line 502 "grammar.y" /* yacc.c:1646  */
    { (yyval.string) = (yyvsp[0].string); }
#line 1980 "grammar.c" /* yacc.c:1646  */
    break;

  case 33:
#line 503 "grammar.y" /* yacc.c:1646  */
    { (yyval.string) = (yyvsp[-1].string); }
#line 1986 "grammar.c" /* yacc.c:1646  */
    break;

  case 34:
#line 509 "grammar.y" /* yacc.c:1646  */
    {
        compiler->error_line = yyget_lineno(yyscanner);
      }
#line 1994 "grammar.c" /* yacc.c:1646  */
    break;

  case 35:
#line 513 "grammar.y" /* yacc.c:1646  */
    {
        (yyval.string) = yr_parser_reduce_string_declaration(
            yyscanner, (int32_t) (yyvsp[0].integer), (yyvsp[-4].c_string), (yyvsp[-1].sized_string));

        yr_free((yyvsp[-4].c_string));
        yr_free((yyvsp[-1].sized_string));

        ERROR_IF((yyval.string) == NULL);
        compiler->error_line = 0;
      }
#line 2009 "grammar.c" /* yacc.c:1646  */
    break;

  case 36:
#line 524 "grammar.y" /* yacc.c:1646  */
    {
        compiler->error_line = yyget_lineno(yyscanner);
      }
#line 2017 "grammar.c" /* yacc.c:1646  */
    break;

  case 37:
#line 528 "grammar.y" /* yacc.c:1646  */
    {
        (yyval.string) = yr_parser_reduce_string_declaration(
            yyscanner, (int32_t) (yyvsp[0].integer) | STRING_GFLAGS_REGEXP, (yyvsp[-4].c_string), (yyvsp[-1].sized_string));

        yr_free((yyvsp[-4].c_string));
        yr_free((yyvsp[-1].sized_string));

        ERROR_IF((yyval.string) == NULL);

        compiler->error_line = 0;
      }
#line 2033 "grammar.c" /* yacc.c:1646  */
    break;

  case 38:
#line 540 "grammar.y" /* yacc.c:1646  */
    {
        (yyval.string) = yr_parser_reduce_string_declaration(
            yyscanner, STRING_GFLAGS_HEXADECIMAL, (yyvsp[-2].c_string), (yyvsp[0].sized_string));

        yr_free((yyvsp[-2].c_string));
        yr_free((yyvsp[0].sized_string));

        ERROR_IF((yyval.string) == NULL);
      }
#line 2047 "grammar.c" /* yacc.c:1646  */
    break;

  case 39:
#line 553 "grammar.y" /* yacc.c:1646  */
    { (yyval.integer) = 0; }
#line 2053 "grammar.c" /* yacc.c:1646  */
    break;

  case 40:
#line 554 "grammar.y" /* yacc.c:1646  */
    { (yyval.integer) = (yyvsp[-1].integer) | (yyvsp[0].integer); }
#line 2059 "grammar.c" /* yacc.c:1646  */
    break;

  case 41:
#line 559 "grammar.y" /* yacc.c:1646  */
    { (yyval.integer) = STRING_GFLAGS_WIDE; }
#line 2065 "grammar.c" /* yacc.c:1646  */
    break;

  case 42:
#line 560 "grammar.y" /* yacc.c:1646  */
    { (yyval.integer) = STRING_GFLAGS_ASCII; }
#line 2071 "grammar.c" /* yacc.c:1646  */
    break;

  case 43:
#line 561 "grammar.y" /* yacc.c:1646  */
    { (yyval.integer) = STRING_GFLAGS_NO_CASE; }
#line 2077 "grammar.c" /* yacc.c:1646  */
    break;

  case 44:
#line 562 "grammar.y" /* yacc.c:1646  */
    { (yyval.integer) = STRING_GFLAGS_FULL_WORD; }
#line 2083 "grammar.c" /* yacc.c:1646  */
    break;

  case 45:
#line 568 "grammar.y" /* yacc.c:1646  */
    {
        int var_index = yr_parser_lookup_loop_variable(yyscanner, (yyvsp[0].c_string));

        if (var_index >= 0)
        {
          compiler->last_result = yr_parser_emit_with_arg(
              yyscanner,
              OP_PUSH_M,
              LOOP_LOCAL_VARS * var_index,
              NULL,
              NULL);

          (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
          (yyval.expression).value.integer = UNDEFINED;
          (yyval.expression).identifier = compiler->loop_identifier[var_index];
        }
        else
        {

          YR_OBJECT* object = (YR_OBJECT*) yr_hash_table_lookup(
              compiler->objects_table, (yyvsp[0].c_string), NULL);

          if (object == NULL)
          {
            char* ns = compiler->current_namespace->name;

            object = (YR_OBJECT*) yr_hash_table_lookup(
                compiler->objects_table, (yyvsp[0].c_string), ns);
          }

          if (object != NULL)
          {
            char* id;

            compiler->last_result = yr_arena_write_string(
                compiler->sz_arena, (yyvsp[0].c_string), &id);

            if (compiler->last_result == ERROR_SUCCESS)
              compiler->last_result = yr_parser_emit_with_arg_reloc(
                  yyscanner,
                  OP_OBJ_LOAD,
                  id,
                  NULL,
                  NULL);

            (yyval.expression).type = EXPRESSION_TYPE_OBJECT;
            (yyval.expression).value.object = object;
            (yyval.expression).identifier = object->identifier;
          }
          else
          {
            YR_RULE* rule = (YR_RULE*) yr_hash_table_lookup(
                compiler->rules_table,
                (yyvsp[0].c_string),
                compiler->current_namespace->name);

            if (rule != NULL)
            {
              compiler->last_result = yr_parser_emit_with_arg_reloc(
                  yyscanner,
                  OP_PUSH_RULE,
                  rule,
                  NULL,
                  NULL);

              (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
              (yyval.expression).value.integer = UNDEFINED;
              (yyval.expression).identifier = rule->identifier;
            }
            else
            {
              yr_compiler_set_error_extra_info(compiler, (yyvsp[0].c_string));
              compiler->last_result = ERROR_UNDEFINED_IDENTIFIER;
            }
          }
        }

        yr_free((yyvsp[0].c_string));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
      }
#line 2172 "grammar.c" /* yacc.c:1646  */
    break;

  case 46:
#line 653 "grammar.y" /* yacc.c:1646  */
    {
        YR_OBJECT* field = NULL;

        if ((yyvsp[-2].expression).type == EXPRESSION_TYPE_OBJECT &&
            (yyvsp[-2].expression).value.object->type == OBJECT_TYPE_STRUCTURE)
        {
          field = yr_object_lookup_field((yyvsp[-2].expression).value.object, (yyvsp[0].c_string));

          if (field != NULL)
          {
            char* ident;

            compiler->last_result = yr_arena_write_string(
              compiler->sz_arena, (yyvsp[0].c_string), &ident);

            if (compiler->last_result == ERROR_SUCCESS)
              compiler->last_result = yr_parser_emit_with_arg_reloc(
                  yyscanner,
                  OP_OBJ_FIELD,
                  ident,
                  NULL,
                  NULL);

            (yyval.expression).type = EXPRESSION_TYPE_OBJECT;
            (yyval.expression).value.object = field;
            (yyval.expression).identifier = field->identifier;
          }
          else
          {
            yr_compiler_set_error_extra_info(compiler, (yyvsp[0].c_string));
            compiler->last_result = ERROR_INVALID_FIELD_NAME;
          }
        }
        else
        {
          yr_compiler_set_error_extra_info(
              compiler, (yyvsp[-2].expression).identifier);

          compiler->last_result = ERROR_NOT_A_STRUCTURE;
        }

        yr_free((yyvsp[0].c_string));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
      }
#line 2222 "grammar.c" /* yacc.c:1646  */
    break;

  case 47:
#line 699 "grammar.y" /* yacc.c:1646  */
    {
        YR_OBJECT_ARRAY* array;
        YR_OBJECT_DICTIONARY* dict;

        if ((yyvsp[-3].expression).type == EXPRESSION_TYPE_OBJECT &&
            (yyvsp[-3].expression).value.object->type == OBJECT_TYPE_ARRAY)
        {
          if ((yyvsp[-1].expression).type != EXPRESSION_TYPE_INTEGER)
          {
            yr_compiler_set_error_extra_info(
                compiler, "array indexes must be of integer type");
            compiler->last_result = ERROR_WRONG_TYPE;
          }

          ERROR_IF(compiler->last_result != ERROR_SUCCESS);

          compiler->last_result = yr_parser_emit(
              yyscanner, OP_INDEX_ARRAY, NULL);

          array = (YR_OBJECT_ARRAY*) (yyvsp[-3].expression).value.object;

          (yyval.expression).type = EXPRESSION_TYPE_OBJECT;
          (yyval.expression).value.object = array->prototype_item;
          (yyval.expression).identifier = array->identifier;
        }
        else if ((yyvsp[-3].expression).type == EXPRESSION_TYPE_OBJECT &&
                 (yyvsp[-3].expression).value.object->type == OBJECT_TYPE_DICTIONARY)
        {
          if ((yyvsp[-1].expression).type != EXPRESSION_TYPE_STRING)
          {
            yr_compiler_set_error_extra_info(
                compiler, "dictionary keys must be of string type");
            compiler->last_result = ERROR_WRONG_TYPE;
          }

          ERROR_IF(compiler->last_result != ERROR_SUCCESS);

          compiler->last_result = yr_parser_emit(
              yyscanner, OP_LOOKUP_DICT, NULL);

          dict = (YR_OBJECT_DICTIONARY*) (yyvsp[-3].expression).value.object;

          (yyval.expression).type = EXPRESSION_TYPE_OBJECT;
          (yyval.expression).value.object = dict->prototype_item;
          (yyval.expression).identifier = dict->identifier;
        }
        else
        {
          yr_compiler_set_error_extra_info(
              compiler, (yyvsp[-3].expression).identifier);

          compiler->last_result = ERROR_NOT_INDEXABLE;
        }

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
      }
#line 2283 "grammar.c" /* yacc.c:1646  */
    break;

  case 48:
#line 757 "grammar.y" /* yacc.c:1646  */
    {
        YR_OBJECT_FUNCTION* function;
        char* args_fmt;

        if ((yyvsp[-3].expression).type == EXPRESSION_TYPE_OBJECT &&
            (yyvsp[-3].expression).value.object->type == OBJECT_TYPE_FUNCTION)
        {
          compiler->last_result = yr_parser_check_types(
              compiler, (YR_OBJECT_FUNCTION*) (yyvsp[-3].expression).value.object, (yyvsp[-1].c_string));

          if (compiler->last_result == ERROR_SUCCESS)
            compiler->last_result = yr_arena_write_string(
              compiler->sz_arena, (yyvsp[-1].c_string), &args_fmt);

          if (compiler->last_result == ERROR_SUCCESS)
            compiler->last_result = yr_parser_emit_with_arg_reloc(
                yyscanner,
                OP_CALL,
                args_fmt,
                NULL,
                NULL);

          function = (YR_OBJECT_FUNCTION*) (yyvsp[-3].expression).value.object;

          (yyval.expression).type = EXPRESSION_TYPE_OBJECT;
          (yyval.expression).value.object = function->return_obj;
          (yyval.expression).identifier = function->identifier;
        }
        else
        {
          yr_compiler_set_error_extra_info(
              compiler, (yyvsp[-3].expression).identifier);

          compiler->last_result = ERROR_NOT_A_FUNCTION;
        }

        yr_free((yyvsp[-1].c_string));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
      }
#line 2328 "grammar.c" /* yacc.c:1646  */
    break;

  case 49:
#line 801 "grammar.y" /* yacc.c:1646  */
    { (yyval.c_string) = yr_strdup(""); }
#line 2334 "grammar.c" /* yacc.c:1646  */
    break;

  case 50:
#line 802 "grammar.y" /* yacc.c:1646  */
    { (yyval.c_string) = (yyvsp[0].c_string); }
#line 2340 "grammar.c" /* yacc.c:1646  */
    break;

  case 51:
#line 807 "grammar.y" /* yacc.c:1646  */
    {
        (yyval.c_string) = (char*) yr_malloc(MAX_FUNCTION_ARGS + 1);

        switch((yyvsp[0].expression).type)
        {
          case EXPRESSION_TYPE_INTEGER:
            strlcpy((yyval.c_string), "i", MAX_FUNCTION_ARGS);
            break;
          case EXPRESSION_TYPE_FLOAT:
            strlcpy((yyval.c_string), "f", MAX_FUNCTION_ARGS);
            break;
          case EXPRESSION_TYPE_BOOLEAN:
            strlcpy((yyval.c_string), "b", MAX_FUNCTION_ARGS);
            break;
          case EXPRESSION_TYPE_STRING:
            strlcpy((yyval.c_string), "s", MAX_FUNCTION_ARGS);
            break;
           case EXPRESSION_TYPE_REGEXP:
             strlcpy((yyval.c_string), "r", MAX_FUNCTION_ARGS);
             break;
          default:
            assert(FALSE);
         }
 
         ERROR_IF((yyval.c_string) == NULL);
       }
#line 2371 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 52:
#line 834 "grammar.y" /* yacc.c:1646  */
     {
         if (strlen((yyvsp[-2].c_string)) == MAX_FUNCTION_ARGS)
         {
          compiler->last_result = ERROR_TOO_MANY_ARGUMENTS;
        }
        else
        {
          switch((yyvsp[0].expression).type)
          {
            case EXPRESSION_TYPE_INTEGER:
              strlcat((yyvsp[-2].c_string), "i", MAX_FUNCTION_ARGS);
              break;
            case EXPRESSION_TYPE_FLOAT:
              strlcat((yyvsp[-2].c_string), "f", MAX_FUNCTION_ARGS);
              break;
            case EXPRESSION_TYPE_BOOLEAN:
              strlcat((yyvsp[-2].c_string), "b", MAX_FUNCTION_ARGS);
              break;
            case EXPRESSION_TYPE_STRING:
              strlcat((yyvsp[-2].c_string), "s", MAX_FUNCTION_ARGS);
              break;
             case EXPRESSION_TYPE_REGEXP:
               strlcat((yyvsp[-2].c_string), "r", MAX_FUNCTION_ARGS);
               break;
            default:
              assert(FALSE);
           }
         }
 
         ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.c_string) = (yyvsp[-2].c_string);
       }
#line 2409 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 53:
#line 872 "grammar.y" /* yacc.c:1646  */
     {
         SIZED_STRING* sized_string = (yyvsp[0].sized_string);
         RE* re;
        RE_ERROR error;

        int re_flags = 0;

        if (sized_string->flags & SIZED_STRING_FLAGS_NO_CASE)
          re_flags |= RE_FLAGS_NO_CASE;

        if (sized_string->flags & SIZED_STRING_FLAGS_DOT_ALL)
          re_flags |= RE_FLAGS_DOT_ALL;

        compiler->last_result = yr_re_compile(
            sized_string->c_string,
            re_flags,
            compiler->re_code_arena,
            &re,
            &error);

        yr_free((yyvsp[0].sized_string));

        if (compiler->last_result == ERROR_INVALID_REGULAR_EXPRESSION)
          yr_compiler_set_error_extra_info(compiler, error.message);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        if (compiler->last_result == ERROR_SUCCESS)
          compiler->last_result = yr_parser_emit_with_arg_reloc(
              yyscanner,
              OP_PUSH,
              re->root_node->forward_code,
              NULL,
              NULL);

        yr_re_destroy(re);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_REGEXP;
       }
#line 2455 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 54:
#line 918 "grammar.y" /* yacc.c:1646  */
     {
         if ((yyvsp[0].expression).type == EXPRESSION_TYPE_STRING)
         {
          if ((yyvsp[0].expression).value.sized_string != NULL)
          {
            yywarning(yyscanner,
              "Using literal string \"%s\" in a boolean operation.",
              (yyvsp[0].expression).value.sized_string->c_string);
          }

          compiler->last_result = yr_parser_emit(
              yyscanner, OP_STR_TO_BOOL, NULL);

          ERROR_IF(compiler->last_result != ERROR_SUCCESS);
        }
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 2478 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 55:
#line 940 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_emit_with_arg(
             yyscanner, OP_PUSH, 1, NULL, NULL);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 2491 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 56:
#line 949 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_emit_with_arg(
             yyscanner, OP_PUSH, 0, NULL, NULL);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 2504 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 57:
#line 958 "grammar.y" /* yacc.c:1646  */
     {
         CHECK_TYPE((yyvsp[-2].expression), EXPRESSION_TYPE_STRING, "matches");
         CHECK_TYPE((yyvsp[0].expression), EXPRESSION_TYPE_REGEXP, "matches");

        if (compiler->last_result == ERROR_SUCCESS)
          compiler->last_result = yr_parser_emit(
              yyscanner,
              OP_MATCHES,
              NULL);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 2523 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 58:
#line 973 "grammar.y" /* yacc.c:1646  */
     {
         CHECK_TYPE((yyvsp[-2].expression), EXPRESSION_TYPE_STRING, "contains");
         CHECK_TYPE((yyvsp[0].expression), EXPRESSION_TYPE_STRING, "contains");

        compiler->last_result = yr_parser_emit(
            yyscanner, OP_CONTAINS, NULL);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 2539 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 59:
#line 985 "grammar.y" /* yacc.c:1646  */
     {
         int result = yr_parser_reduce_string_identifier(
             yyscanner,
            (yyvsp[0].c_string),
            OP_FOUND,
            UNDEFINED);

        yr_free((yyvsp[0].c_string));

        ERROR_IF(result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 2557 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 60:
#line 999 "grammar.y" /* yacc.c:1646  */
     {
         CHECK_TYPE((yyvsp[0].expression), EXPRESSION_TYPE_INTEGER, "at");
 
        compiler->last_result = yr_parser_reduce_string_identifier(
            yyscanner, (yyvsp[-2].c_string), OP_FOUND_AT, (yyvsp[0].expression).value.integer);

        yr_free((yyvsp[-2].c_string));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 2574 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 61:
#line 1012 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_string_identifier(
             yyscanner, (yyvsp[-2].c_string), OP_FOUND_IN, UNDEFINED);

        yr_free((yyvsp[-2].c_string));

        ERROR_IF(compiler->last_result!= ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 2589 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 62:
#line 1023 "grammar.y" /* yacc.c:1646  */
     {
         if (compiler->loop_depth > 0)
         {
           compiler->loop_depth--;
           compiler->loop_identifier[compiler->loop_depth] = NULL;
         }

        YYERROR;
       }
#line 2603 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 63:
#line 1033 "grammar.y" /* yacc.c:1646  */
     {
         int var_index;
 
        if (compiler->loop_depth == MAX_LOOP_NESTING)
          compiler->last_result = \
              ERROR_LOOP_NESTING_LIMIT_EXCEEDED;

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        var_index = yr_parser_lookup_loop_variable(
            yyscanner, (yyvsp[-1].c_string));

        if (var_index >= 0)
        {
          yr_compiler_set_error_extra_info(
              compiler, (yyvsp[-1].c_string));

          compiler->last_result = \
              ERROR_DUPLICATED_LOOP_IDENTIFIER;
        }

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        compiler->last_result = yr_parser_emit_with_arg(
            yyscanner, OP_PUSH, UNDEFINED, NULL, NULL);
 
         ERROR_IF(compiler->last_result != ERROR_SUCCESS);
       }
#line 2637 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 64:
#line 1063 "grammar.y" /* yacc.c:1646  */
     {
         int mem_offset = LOOP_LOCAL_VARS * compiler->loop_depth;
         uint8_t* addr;

        yr_parser_emit_with_arg(
            yyscanner, OP_CLEAR_M, mem_offset + 1, NULL, NULL);

        yr_parser_emit_with_arg(
            yyscanner, OP_CLEAR_M, mem_offset + 2, NULL, NULL);

        if ((yyvsp[-1].integer) == INTEGER_SET_ENUMERATION)
        {
          yr_parser_emit_with_arg(
              yyscanner, OP_POP_M, mem_offset, &addr, NULL);
        }
        else // INTEGER_SET_RANGE
        {
          yr_parser_emit_with_arg(
              yyscanner, OP_POP_M, mem_offset + 3, &addr, NULL);

          yr_parser_emit_with_arg(
              yyscanner, OP_POP_M, mem_offset, NULL, NULL);
        }

        compiler->loop_address[compiler->loop_depth] = addr;
         compiler->loop_identifier[compiler->loop_depth] = (yyvsp[-4].c_string);
         compiler->loop_depth++;
       }
#line 2676 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 65:
#line 1098 "grammar.y" /* yacc.c:1646  */
     {
         int mem_offset;
 
        compiler->loop_depth--;
        mem_offset = LOOP_LOCAL_VARS * compiler->loop_depth;


        yr_parser_emit_with_arg(
            yyscanner, OP_ADD_M, mem_offset + 1, NULL, NULL);

        yr_parser_emit_with_arg(
            yyscanner, OP_INCR_M, mem_offset + 2, NULL, NULL);

        if ((yyvsp[-5].integer) == INTEGER_SET_ENUMERATION)
        {
          yr_parser_emit_with_arg_reloc(
              yyscanner,
              OP_JNUNDEF,
              compiler->loop_address[compiler->loop_depth],
              NULL,
              NULL);
        }
        else // INTEGER_SET_RANGE
        {
          yr_parser_emit_with_arg(
              yyscanner, OP_INCR_M, mem_offset, NULL, NULL);

          yr_parser_emit_with_arg(
              yyscanner, OP_PUSH_M, mem_offset, NULL, NULL);

          yr_parser_emit_with_arg(
              yyscanner, OP_PUSH_M, mem_offset + 3, NULL, NULL);

          yr_parser_emit_with_arg_reloc(
              yyscanner,
              OP_JLE,
              compiler->loop_address[compiler->loop_depth],
              NULL,
              NULL);

          yr_parser_emit(yyscanner, OP_POP, NULL);
          yr_parser_emit(yyscanner, OP_POP, NULL);
        }

        yr_parser_emit(yyscanner, OP_POP, NULL);

        yr_parser_emit_with_arg(
            yyscanner, OP_SWAPUNDEF, mem_offset + 2, NULL, NULL);

        yr_parser_emit_with_arg(
            yyscanner, OP_PUSH_M, mem_offset + 1, NULL, NULL);

        yr_parser_emit(yyscanner, OP_INT_LE, NULL);

        compiler->loop_identifier[compiler->loop_depth] = NULL;
        yr_free((yyvsp[-8].c_string));
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 2759 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 66:
#line 1177 "grammar.y" /* yacc.c:1646  */
     {
         int mem_offset = LOOP_LOCAL_VARS * compiler->loop_depth;
         uint8_t* addr;

        if (compiler->loop_depth == MAX_LOOP_NESTING)
          compiler->last_result = \
            ERROR_LOOP_NESTING_LIMIT_EXCEEDED;

        if (compiler->loop_for_of_mem_offset != -1)
          compiler->last_result = \
            ERROR_NESTED_FOR_OF_LOOP;

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        yr_parser_emit_with_arg(
            yyscanner, OP_CLEAR_M, mem_offset + 1, NULL, NULL);

        yr_parser_emit_with_arg(
            yyscanner, OP_CLEAR_M, mem_offset + 2, NULL, NULL);

        yr_parser_emit_with_arg(
            yyscanner, OP_POP_M, mem_offset, &addr, NULL);

        compiler->loop_for_of_mem_offset = mem_offset;
        compiler->loop_address[compiler->loop_depth] = addr;
         compiler->loop_identifier[compiler->loop_depth] = NULL;
         compiler->loop_depth++;
       }
#line 2793 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 67:
#line 1207 "grammar.y" /* yacc.c:1646  */
     {
         int mem_offset;
 
        compiler->loop_depth--;
        compiler->loop_for_of_mem_offset = -1;

        mem_offset = LOOP_LOCAL_VARS * compiler->loop_depth;


        yr_parser_emit_with_arg(
            yyscanner, OP_ADD_M, mem_offset + 1, NULL, NULL);

        yr_parser_emit_with_arg(
            yyscanner, OP_INCR_M, mem_offset + 2, NULL, NULL);

        yr_parser_emit_with_arg_reloc(
            yyscanner,
            OP_JNUNDEF,
            compiler->loop_address[compiler->loop_depth],
            NULL,
            NULL);

        yr_parser_emit(yyscanner, OP_POP, NULL);

        yr_parser_emit_with_arg(
            yyscanner, OP_SWAPUNDEF, mem_offset + 2, NULL, NULL);

        yr_parser_emit_with_arg(
            yyscanner, OP_PUSH_M, mem_offset + 1, NULL, NULL);

        yr_parser_emit(yyscanner, OP_INT_LE, NULL);

         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
 
       }
#line 2846 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 68:
#line 1256 "grammar.y" /* yacc.c:1646  */
     {
         yr_parser_emit(yyscanner, OP_OF, NULL);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 2856 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 69:
#line 1262 "grammar.y" /* yacc.c:1646  */
     {
         yr_parser_emit(yyscanner, OP_NOT, NULL);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 2866 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 70:
#line 1268 "grammar.y" /* yacc.c:1646  */
     {
         YR_FIXUP* fixup;
         void* jmp_destination_addr;

        compiler->last_result = yr_parser_emit_with_arg_reloc(
            yyscanner,
            OP_JFALSE,
            0,          // still don't know the jump destination
            NULL,
            &jmp_destination_addr);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        fixup = (YR_FIXUP*) yr_malloc(sizeof(YR_FIXUP));

        if (fixup == NULL)
          compiler->last_error = ERROR_INSUFFICIENT_MEMORY;

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        fixup->address = jmp_destination_addr;
         fixup->next = compiler->fixup_stack_head;
         compiler->fixup_stack_head = fixup;
       }
#line 2896 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 71:
#line 1294 "grammar.y" /* yacc.c:1646  */
     {
         YR_FIXUP* fixup;
         uint8_t* and_addr;


        compiler->last_result = yr_arena_reserve_memory(
            compiler->code_arena, 2);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        compiler->last_result = yr_parser_emit(yyscanner, OP_AND, &and_addr);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);


        fixup = compiler->fixup_stack_head;


        *(void**)(fixup->address) = (void*)(and_addr + 1);

        compiler->fixup_stack_head = fixup->next;
        yr_free(fixup);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 2936 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 72:
#line 1330 "grammar.y" /* yacc.c:1646  */
     {
         YR_FIXUP* fixup;
         void* jmp_destination_addr;

        compiler->last_result = yr_parser_emit_with_arg_reloc(
            yyscanner,
            OP_JTRUE,
            0,         // still don't know the jump destination
            NULL,
            &jmp_destination_addr);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        fixup = (YR_FIXUP*) yr_malloc(sizeof(YR_FIXUP));

        if (fixup == NULL)
          compiler->last_error = ERROR_INSUFFICIENT_MEMORY;

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        fixup->address = jmp_destination_addr;
         fixup->next = compiler->fixup_stack_head;
         compiler->fixup_stack_head = fixup;
       }
#line 2965 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 73:
#line 1355 "grammar.y" /* yacc.c:1646  */
     {
         YR_FIXUP* fixup;
         uint8_t* or_addr;


        compiler->last_result = yr_arena_reserve_memory(
            compiler->code_arena, 2);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        compiler->last_result = yr_parser_emit(yyscanner, OP_OR, &or_addr);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);


        fixup = compiler->fixup_stack_head;


        *(void**)(fixup->address) = (void*)(or_addr + 1);

        compiler->fixup_stack_head = fixup->next;
        yr_free(fixup);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 3005 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 74:
#line 1391 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_operation(
             yyscanner, "<", (yyvsp[-2].expression), (yyvsp[0].expression));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 3018 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 75:
#line 1400 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_operation(
             yyscanner, ">", (yyvsp[-2].expression), (yyvsp[0].expression));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 3031 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 76:
#line 1409 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_operation(
             yyscanner, "<=", (yyvsp[-2].expression), (yyvsp[0].expression));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 3044 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 77:
#line 1418 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_operation(
             yyscanner, ">=", (yyvsp[-2].expression), (yyvsp[0].expression));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 3057 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 78:
#line 1427 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_operation(
             yyscanner, "==", (yyvsp[-2].expression), (yyvsp[0].expression));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 3070 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 79:
#line 1436 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_operation(
             yyscanner, "!=", (yyvsp[-2].expression), (yyvsp[0].expression));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
       }
#line 3083 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 80:
#line 1445 "grammar.y" /* yacc.c:1646  */
     {
         (yyval.expression) = (yyvsp[0].expression);
       }
#line 3091 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 81:
#line 1449 "grammar.y" /* yacc.c:1646  */
     {
         (yyval.expression) = (yyvsp[-1].expression);
       }
#line 3099 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 82:
#line 1456 "grammar.y" /* yacc.c:1646  */
     { (yyval.integer) = INTEGER_SET_ENUMERATION; }
#line 3105 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 83:
#line 1457 "grammar.y" /* yacc.c:1646  */
     { (yyval.integer) = INTEGER_SET_RANGE; }
#line 3111 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 84:
#line 1463 "grammar.y" /* yacc.c:1646  */
     {
         if ((yyvsp[-3].expression).type != EXPRESSION_TYPE_INTEGER)
         {
          yr_compiler_set_error_extra_info(
              compiler, "wrong type for range's lower bound");
          compiler->last_result = ERROR_WRONG_TYPE;
        }

        if ((yyvsp[-1].expression).type != EXPRESSION_TYPE_INTEGER)
        {
          yr_compiler_set_error_extra_info(
              compiler, "wrong type for range's upper bound");
          compiler->last_result = ERROR_WRONG_TYPE;
        }
 
         ERROR_IF(compiler->last_result != ERROR_SUCCESS);
       }
#line 3133 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 85:
#line 1485 "grammar.y" /* yacc.c:1646  */
     {
         if ((yyvsp[0].expression).type != EXPRESSION_TYPE_INTEGER)
         {
          yr_compiler_set_error_extra_info(
              compiler, "wrong type for enumeration item");
          compiler->last_result = ERROR_WRONG_TYPE;

        }
 
         ERROR_IF(compiler->last_result != ERROR_SUCCESS);
       }
#line 3149 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 86:
#line 1497 "grammar.y" /* yacc.c:1646  */
     {
         if ((yyvsp[0].expression).type != EXPRESSION_TYPE_INTEGER)
         {
          yr_compiler_set_error_extra_info(
              compiler, "wrong type for enumeration item");
          compiler->last_result = ERROR_WRONG_TYPE;
        }
 
         ERROR_IF(compiler->last_result != ERROR_SUCCESS);
       }
#line 3164 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 87:
#line 1512 "grammar.y" /* yacc.c:1646  */
     {
         yr_parser_emit_with_arg(yyscanner, OP_PUSH, UNDEFINED, NULL, NULL);
       }
#line 3173 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 89:
#line 1518 "grammar.y" /* yacc.c:1646  */
     {
         yr_parser_emit_with_arg(yyscanner, OP_PUSH, UNDEFINED, NULL, NULL);
         yr_parser_emit_pushes_for_strings(yyscanner, "$*");
 
         ERROR_IF(compiler->last_result != ERROR_SUCCESS);
       }
#line 3184 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 92:
#line 1535 "grammar.y" /* yacc.c:1646  */
     {
         yr_parser_emit_pushes_for_strings(yyscanner, (yyvsp[0].c_string));
         yr_free((yyvsp[0].c_string));
 
         ERROR_IF(compiler->last_result != ERROR_SUCCESS);
       }
#line 3195 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 93:
#line 1542 "grammar.y" /* yacc.c:1646  */
     {
         yr_parser_emit_pushes_for_strings(yyscanner, (yyvsp[0].c_string));
         yr_free((yyvsp[0].c_string));
 
         ERROR_IF(compiler->last_result != ERROR_SUCCESS);
       }
#line 3206 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 95:
#line 1554 "grammar.y" /* yacc.c:1646  */
     {
         yr_parser_emit_with_arg(yyscanner, OP_PUSH, UNDEFINED, NULL, NULL);
       }
#line 3214 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 96:
#line 1558 "grammar.y" /* yacc.c:1646  */
     {
         yr_parser_emit_with_arg(yyscanner, OP_PUSH, 1, NULL, NULL);
       }
#line 3222 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 97:
#line 1566 "grammar.y" /* yacc.c:1646  */
     {
         (yyval.expression) = (yyvsp[-1].expression);
       }
#line 3230 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 98:
#line 1570 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_emit(
             yyscanner, OP_FILESIZE, NULL);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = UNDEFINED;
       }
#line 3244 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 99:
#line 1580 "grammar.y" /* yacc.c:1646  */
     {
         yywarning(yyscanner,
             "Using deprecated \"entrypoint\" keyword. Use the \"entry_point\" "
            "function from PE module instead.");

        compiler->last_result = yr_parser_emit(
            yyscanner, OP_ENTRYPOINT, NULL);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = UNDEFINED;
       }
#line 3262 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 100:
#line 1594 "grammar.y" /* yacc.c:1646  */
     {
         CHECK_TYPE((yyvsp[-1].expression), EXPRESSION_TYPE_INTEGER, "intXXXX or uintXXXX");
 

        compiler->last_result = yr_parser_emit(
            yyscanner, (uint8_t) (OP_READ_INT + (yyvsp[-3].integer)), NULL);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = UNDEFINED;
       }
#line 3282 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 101:
#line 1610 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_emit_with_arg(
             yyscanner, OP_PUSH, (yyvsp[0].integer), NULL, NULL);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = (yyvsp[0].integer);
       }
#line 3296 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 102:
#line 1620 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_emit_with_arg_double(
             yyscanner, OP_PUSH, (yyvsp[0].double_), NULL, NULL);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);
 
         (yyval.expression).type = EXPRESSION_TYPE_FLOAT;
       }
#line 3309 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 103:
#line 1629 "grammar.y" /* yacc.c:1646  */
     {
         SIZED_STRING* sized_string;
 
        compiler->last_result = yr_arena_write_data(
            compiler->sz_arena,
            (yyvsp[0].sized_string),
            (yyvsp[0].sized_string)->length + sizeof(SIZED_STRING),
            (void**) &sized_string);

        yr_free((yyvsp[0].sized_string));

        if (compiler->last_result == ERROR_SUCCESS)
          compiler->last_result = yr_parser_emit_with_arg_reloc(
              yyscanner,
              OP_PUSH,
              sized_string,
              NULL,
              NULL);

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

         (yyval.expression).type = EXPRESSION_TYPE_STRING;
         (yyval.expression).value.sized_string = sized_string;
       }
#line 3338 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 104:
#line 1654 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_string_identifier(
             yyscanner, (yyvsp[0].c_string), OP_COUNT, UNDEFINED);

        yr_free((yyvsp[0].c_string));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = UNDEFINED;
       }
#line 3354 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 105:
#line 1666 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_string_identifier(
             yyscanner, (yyvsp[-3].c_string), OP_OFFSET, UNDEFINED);

        yr_free((yyvsp[-3].c_string));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = UNDEFINED;
       }
#line 3370 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 106:
#line 1678 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_emit_with_arg(
             yyscanner, OP_PUSH, 1, NULL, NULL);

        if (compiler->last_result == ERROR_SUCCESS)
          compiler->last_result = yr_parser_reduce_string_identifier(
              yyscanner, (yyvsp[0].c_string), OP_OFFSET, UNDEFINED);

        yr_free((yyvsp[0].c_string));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = UNDEFINED;
       }
#line 3390 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 107:
#line 1694 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_string_identifier(
             yyscanner, (yyvsp[-3].c_string), OP_LENGTH, UNDEFINED);

        yr_free((yyvsp[-3].c_string));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = UNDEFINED;
       }
#line 3406 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 108:
#line 1706 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_emit_with_arg(
             yyscanner, OP_PUSH, 1, NULL, NULL);

        if (compiler->last_result == ERROR_SUCCESS)
          compiler->last_result = yr_parser_reduce_string_identifier(
              yyscanner, (yyvsp[0].c_string), OP_LENGTH, UNDEFINED);

        yr_free((yyvsp[0].c_string));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = UNDEFINED;
       }
#line 3426 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 109:
#line 1722 "grammar.y" /* yacc.c:1646  */
     {
         if ((yyvsp[0].expression).type == EXPRESSION_TYPE_INTEGER)  // loop identifier
         {
          (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
          (yyval.expression).value.integer = UNDEFINED;
        }
        else if ((yyvsp[0].expression).type == EXPRESSION_TYPE_BOOLEAN)  // rule identifier
        {
          (yyval.expression).type = EXPRESSION_TYPE_BOOLEAN;
          (yyval.expression).value.integer = UNDEFINED;
        }
        else if ((yyvsp[0].expression).type == EXPRESSION_TYPE_OBJECT)
        {
          compiler->last_result = yr_parser_emit(
              yyscanner, OP_OBJ_VALUE, NULL);

          switch((yyvsp[0].expression).value.object->type)
          {
            case OBJECT_TYPE_INTEGER:
              (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
              (yyval.expression).value.integer = UNDEFINED;
              break;
            case OBJECT_TYPE_FLOAT:
              (yyval.expression).type = EXPRESSION_TYPE_FLOAT;
              break;
            case OBJECT_TYPE_STRING:
              (yyval.expression).type = EXPRESSION_TYPE_STRING;
              (yyval.expression).value.sized_string = NULL;
              break;
            default:
              yr_compiler_set_error_extra_info_fmt(
                  compiler,
                  "wrong usage of identifier \"%s\"",
                  (yyvsp[0].expression).identifier);
              compiler->last_result = ERROR_WRONG_TYPE;
          }
        }
        else
        {
          assert(FALSE);
        }
 
         ERROR_IF(compiler->last_result != ERROR_SUCCESS);
       }
#line 3475 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 110:
#line 1767 "grammar.y" /* yacc.c:1646  */
     {
         CHECK_TYPE((yyvsp[0].expression), EXPRESSION_TYPE_INTEGER | EXPRESSION_TYPE_FLOAT, "-");
 
        if ((yyvsp[0].expression).type == EXPRESSION_TYPE_INTEGER)
        {
          (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
          (yyval.expression).value.integer = ((yyvsp[0].expression).value.integer == UNDEFINED) ?
              UNDEFINED : -((yyvsp[0].expression).value.integer);
          compiler->last_result = yr_parser_emit(yyscanner, OP_INT_MINUS, NULL);
        }
        else if ((yyvsp[0].expression).type == EXPRESSION_TYPE_FLOAT)
        {
          (yyval.expression).type = EXPRESSION_TYPE_FLOAT;
          compiler->last_result = yr_parser_emit(yyscanner, OP_DBL_MINUS, NULL);
        }
 
         ERROR_IF(compiler->last_result != ERROR_SUCCESS);
       }
#line 3498 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 111:
#line 1786 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_operation(
             yyscanner, "+", (yyvsp[-2].expression), (yyvsp[0].expression));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        if ((yyvsp[-2].expression).type == EXPRESSION_TYPE_INTEGER &&
            (yyvsp[0].expression).type == EXPRESSION_TYPE_INTEGER)
        {
          (yyval.expression).value.integer = OPERATION(+, (yyvsp[-2].expression).value.integer, (yyvsp[0].expression).value.integer);
          (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
        }
        else
        {
           (yyval.expression).type = EXPRESSION_TYPE_FLOAT;
         }
       }
#line 3520 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 112:
#line 1804 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_operation(
             yyscanner, "-", (yyvsp[-2].expression), (yyvsp[0].expression));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        if ((yyvsp[-2].expression).type == EXPRESSION_TYPE_INTEGER &&
            (yyvsp[0].expression).type == EXPRESSION_TYPE_INTEGER)
        {
          (yyval.expression).value.integer = OPERATION(-, (yyvsp[-2].expression).value.integer, (yyvsp[0].expression).value.integer);
          (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
        }
        else
        {
           (yyval.expression).type = EXPRESSION_TYPE_FLOAT;
         }
       }
#line 3542 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 113:
#line 1822 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_operation(
             yyscanner, "*", (yyvsp[-2].expression), (yyvsp[0].expression));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        if ((yyvsp[-2].expression).type == EXPRESSION_TYPE_INTEGER &&
            (yyvsp[0].expression).type == EXPRESSION_TYPE_INTEGER)
        {
          (yyval.expression).value.integer = OPERATION(*, (yyvsp[-2].expression).value.integer, (yyvsp[0].expression).value.integer);
          (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
        }
        else
        {
           (yyval.expression).type = EXPRESSION_TYPE_FLOAT;
         }
       }
#line 3564 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 114:
#line 1840 "grammar.y" /* yacc.c:1646  */
     {
         compiler->last_result = yr_parser_reduce_operation(
             yyscanner, "\\", (yyvsp[-2].expression), (yyvsp[0].expression));

        ERROR_IF(compiler->last_result != ERROR_SUCCESS);

        if ((yyvsp[-2].expression).type == EXPRESSION_TYPE_INTEGER &&
            (yyvsp[0].expression).type == EXPRESSION_TYPE_INTEGER)
        {
          if ((yyvsp[0].expression).value.integer != 0)
          {
            (yyval.expression).value.integer = OPERATION(/, (yyvsp[-2].expression).value.integer, (yyvsp[0].expression).value.integer);
            (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
          }
          else
          {
            compiler->last_result = ERROR_DIVISION_BY_ZERO;
            ERROR_IF(compiler->last_result != ERROR_SUCCESS);
          }
        }
        else
        {
           (yyval.expression).type = EXPRESSION_TYPE_FLOAT;
         }
       }
#line 3594 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 115:
#line 1866 "grammar.y" /* yacc.c:1646  */
     {
         CHECK_TYPE((yyvsp[-2].expression), EXPRESSION_TYPE_INTEGER, "%");
         CHECK_TYPE((yyvsp[0].expression), EXPRESSION_TYPE_INTEGER, "%");

        yr_parser_emit(yyscanner, OP_MOD, NULL);

        if ((yyvsp[0].expression).value.integer != 0)
        {
          (yyval.expression).value.integer = OPERATION(%, (yyvsp[-2].expression).value.integer, (yyvsp[0].expression).value.integer);
          (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
        }
        else
        {
          compiler->last_result = ERROR_DIVISION_BY_ZERO;
           ERROR_IF(compiler->last_result != ERROR_SUCCESS);
         }
       }
#line 3616 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 116:
#line 1884 "grammar.y" /* yacc.c:1646  */
     {
         CHECK_TYPE((yyvsp[-2].expression), EXPRESSION_TYPE_INTEGER, "^");
         CHECK_TYPE((yyvsp[0].expression), EXPRESSION_TYPE_INTEGER, "^");

        yr_parser_emit(yyscanner, OP_BITWISE_XOR, NULL);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = OPERATION(^, (yyvsp[-2].expression).value.integer, (yyvsp[0].expression).value.integer);
       }
#line 3630 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 117:
#line 1894 "grammar.y" /* yacc.c:1646  */
     {
         CHECK_TYPE((yyvsp[-2].expression), EXPRESSION_TYPE_INTEGER, "^");
         CHECK_TYPE((yyvsp[0].expression), EXPRESSION_TYPE_INTEGER, "^");

        yr_parser_emit(yyscanner, OP_BITWISE_AND, NULL);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = OPERATION(&, (yyvsp[-2].expression).value.integer, (yyvsp[0].expression).value.integer);
       }
#line 3644 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 118:
#line 1904 "grammar.y" /* yacc.c:1646  */
     {
         CHECK_TYPE((yyvsp[-2].expression), EXPRESSION_TYPE_INTEGER, "|");
         CHECK_TYPE((yyvsp[0].expression), EXPRESSION_TYPE_INTEGER, "|");

        yr_parser_emit(yyscanner, OP_BITWISE_OR, NULL);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = OPERATION(|, (yyvsp[-2].expression).value.integer, (yyvsp[0].expression).value.integer);
       }
#line 3658 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 119:
#line 1914 "grammar.y" /* yacc.c:1646  */
     {
         CHECK_TYPE((yyvsp[0].expression), EXPRESSION_TYPE_INTEGER, "~");
 
        yr_parser_emit(yyscanner, OP_BITWISE_NOT, NULL);

        (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = ((yyvsp[0].expression).value.integer == UNDEFINED) ?
             UNDEFINED : ~((yyvsp[0].expression).value.integer);
       }
#line 3672 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 120:
#line 1924 "grammar.y" /* yacc.c:1646  */
     {
         CHECK_TYPE((yyvsp[-2].expression), EXPRESSION_TYPE_INTEGER, "<<");
         CHECK_TYPE((yyvsp[0].expression), EXPRESSION_TYPE_INTEGER, "<<");

        yr_parser_emit(yyscanner, OP_SHL, NULL);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = OPERATION(<<, (yyvsp[-2].expression).value.integer, (yyvsp[0].expression).value.integer);
       }
#line 3686 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 121:
#line 1934 "grammar.y" /* yacc.c:1646  */
     {
         CHECK_TYPE((yyvsp[-2].expression), EXPRESSION_TYPE_INTEGER, ">>");
         CHECK_TYPE((yyvsp[0].expression), EXPRESSION_TYPE_INTEGER, ">>");

        yr_parser_emit(yyscanner, OP_SHR, NULL);

         (yyval.expression).type = EXPRESSION_TYPE_INTEGER;
         (yyval.expression).value.integer = OPERATION(>>, (yyvsp[-2].expression).value.integer, (yyvsp[0].expression).value.integer);
       }
#line 3700 "grammar.c" /* yacc.c:1646  */
     break;
 
   case 122:
#line 1944 "grammar.y" /* yacc.c:1646  */
     {
         (yyval.expression) = (yyvsp[0].expression);
       }
#line 3708 "grammar.c" /* yacc.c:1646  */
     break;
 
 
#line 3712 "grammar.c" /* yacc.c:1646  */
       default: break;
     }
   /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (yyscanner, compiler, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yyscanner, compiler, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, yyscanner, compiler);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yyscanner, compiler);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (yyscanner, compiler, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, yyscanner, compiler);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yyscanner, compiler);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
 #endif
   return yyresult;
 }
