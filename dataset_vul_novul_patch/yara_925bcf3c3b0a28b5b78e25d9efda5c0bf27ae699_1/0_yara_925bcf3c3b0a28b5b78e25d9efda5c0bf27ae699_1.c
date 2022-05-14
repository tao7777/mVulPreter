yyparse (void *yyscanner, RE_LEX_ENVIRONMENT *lex_env)
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
      yychar = yylex (&yylval, yyscanner, lex_env);
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
         case 2:
#line 113 "re_grammar.y" /* yacc.c:1646  */
     {
         RE_AST* re_ast = yyget_extra(yyscanner);
         re_ast->root_node = (yyvsp[0].re_node);
       }
#line 1348 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 4:
#line 122 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = (yyvsp[0].re_node);
       }
#line 1356 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 5:
#line 126 "re_grammar.y" /* yacc.c:1646  */
     {
         mark_as_not_fast_regexp();
        incr_ast_levels();
 
         (yyval.re_node) = yr_re_node_create(RE_NODE_ALT, (yyvsp[-2].re_node), (yyvsp[0].re_node));
 
        DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[-2].re_node));
        DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[0].re_node));
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1372 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 6:
#line 138 "re_grammar.y" /* yacc.c:1646  */
     {
         RE_NODE* node;
 
         mark_as_not_fast_regexp();
        incr_ast_levels();
 
         node = yr_re_node_create(RE_NODE_EMPTY, NULL, NULL);
 
        DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[-1].re_node));
        ERROR_IF(node == NULL, ERROR_INSUFFICIENT_MEMORY);

        (yyval.re_node) = yr_re_node_create(RE_NODE_ALT, (yyvsp[-1].re_node), node);
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1392 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 7:
#line 157 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = (yyvsp[0].re_node);
       }
#line 1400 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 8:
#line 161 "re_grammar.y" /* yacc.c:1646  */
     {
        incr_ast_levels();

         (yyval.re_node) = yr_re_node_create(RE_NODE_CONCAT, (yyvsp[-1].re_node), (yyvsp[0].re_node));
 
         DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[-1].re_node));
         DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[0].re_node));
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1414 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 9:
#line 174 "re_grammar.y" /* yacc.c:1646  */
     {
         RE_AST* re_ast;
 
        mark_as_not_fast_regexp();

        re_ast = yyget_extra(yyscanner);
        re_ast->flags |= RE_FLAGS_GREEDY;

        (yyval.re_node) = yr_re_node_create(RE_NODE_STAR, (yyvsp[-1].re_node), NULL);

         DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[-1].re_node));
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1432 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 10:
#line 188 "re_grammar.y" /* yacc.c:1646  */
     {
         RE_AST* re_ast;
 
        mark_as_not_fast_regexp();

        re_ast = yyget_extra(yyscanner);
        re_ast->flags |= RE_FLAGS_UNGREEDY;

        (yyval.re_node) = yr_re_node_create(RE_NODE_STAR, (yyvsp[-2].re_node), NULL);

        DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[-2].re_node));
        ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
 
         (yyval.re_node)->greedy = FALSE;
       }
#line 1452 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 11:
#line 204 "re_grammar.y" /* yacc.c:1646  */
     {
         RE_AST* re_ast;
 
        mark_as_not_fast_regexp();

        re_ast = yyget_extra(yyscanner);
        re_ast->flags |= RE_FLAGS_GREEDY;

        (yyval.re_node) = yr_re_node_create(RE_NODE_PLUS, (yyvsp[-1].re_node), NULL);

         DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[-1].re_node));
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1470 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 12:
#line 218 "re_grammar.y" /* yacc.c:1646  */
     {
         RE_AST* re_ast;
 
        mark_as_not_fast_regexp();

        re_ast = yyget_extra(yyscanner);
        re_ast->flags |= RE_FLAGS_UNGREEDY;

        (yyval.re_node) = yr_re_node_create(RE_NODE_PLUS, (yyvsp[-2].re_node), NULL);

        DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[-2].re_node));
        ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
 
         (yyval.re_node)->greedy = FALSE;
       }
#line 1490 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 13:
#line 234 "re_grammar.y" /* yacc.c:1646  */
     {
         RE_AST* re_ast = yyget_extra(yyscanner);
         re_ast->flags |= RE_FLAGS_GREEDY;

        if ((yyvsp[-1].re_node)->type == RE_NODE_ANY)
        {
          (yyval.re_node) = yr_re_node_create(RE_NODE_RANGE_ANY, NULL, NULL);
          DESTROY_NODE_IF(TRUE, (yyvsp[-1].re_node));
        }
        else
        {
          mark_as_not_fast_regexp();
          (yyval.re_node) = yr_re_node_create(RE_NODE_RANGE, (yyvsp[-1].re_node), NULL);
          DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[-1].re_node));
        }

        DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[-1].re_node));
        ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);

         (yyval.re_node)->start = 0;
         (yyval.re_node)->end = 1;
       }
#line 1517 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 14:
#line 257 "re_grammar.y" /* yacc.c:1646  */
     {
         RE_AST* re_ast = yyget_extra(yyscanner);
         re_ast->flags |= RE_FLAGS_UNGREEDY;

        if ((yyvsp[-2].re_node)->type == RE_NODE_ANY)
        {
          (yyval.re_node) = yr_re_node_create(RE_NODE_RANGE_ANY, NULL, NULL);
          DESTROY_NODE_IF(TRUE, (yyvsp[-2].re_node));
        }
        else
        {
          mark_as_not_fast_regexp();
          (yyval.re_node) = yr_re_node_create(RE_NODE_RANGE, (yyvsp[-2].re_node), NULL);
          DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[-2].re_node));
        }

        DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[-2].re_node));
        ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);

        (yyval.re_node)->start = 0;
         (yyval.re_node)->end = 1;
         (yyval.re_node)->greedy = FALSE;
       }
#line 1545 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 15:
#line 281 "re_grammar.y" /* yacc.c:1646  */
     {
         RE_AST* re_ast = yyget_extra(yyscanner);
         re_ast->flags |= RE_FLAGS_GREEDY;

        if ((yyvsp[-1].re_node)->type == RE_NODE_ANY)
        {
          (yyval.re_node) = yr_re_node_create(RE_NODE_RANGE_ANY, NULL, NULL);
          DESTROY_NODE_IF(TRUE, (yyvsp[-1].re_node));
        }
        else
        {
          mark_as_not_fast_regexp();
          (yyval.re_node) = yr_re_node_create(RE_NODE_RANGE, (yyvsp[-1].re_node), NULL);
          DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[-1].re_node));
        }

        ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);

         (yyval.re_node)->start = (yyvsp[0].range) & 0xFFFF;;
         (yyval.re_node)->end = (yyvsp[0].range) >> 16;;
       }
#line 1571 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 16:
#line 303 "re_grammar.y" /* yacc.c:1646  */
     {
         RE_AST* re_ast = yyget_extra(yyscanner);
         re_ast->flags |= RE_FLAGS_UNGREEDY;

        if ((yyvsp[-2].re_node)->type == RE_NODE_ANY)
        {
          (yyval.re_node) = yr_re_node_create(RE_NODE_RANGE_ANY, NULL, NULL);
          DESTROY_NODE_IF(TRUE, (yyvsp[-2].re_node));
        }
        else
        {
          mark_as_not_fast_regexp();
          (yyval.re_node) = yr_re_node_create(RE_NODE_RANGE, (yyvsp[-2].re_node), NULL);
          DESTROY_NODE_IF((yyval.re_node) == NULL, (yyvsp[-2].re_node));
        }

        ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);

        (yyval.re_node)->start = (yyvsp[-1].range) & 0xFFFF;;
         (yyval.re_node)->end = (yyvsp[-1].range) >> 16;;
         (yyval.re_node)->greedy = FALSE;
       }
#line 1598 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 17:
#line 326 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = (yyvsp[0].re_node);
       }
#line 1606 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 18:
#line 330 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = yr_re_node_create(RE_NODE_WORD_BOUNDARY, NULL, NULL);
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1616 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 19:
#line 336 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = yr_re_node_create(RE_NODE_NON_WORD_BOUNDARY, NULL, NULL);
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1626 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 20:
#line 342 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = yr_re_node_create(RE_NODE_ANCHOR_START, NULL, NULL);
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1636 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 21:
#line 348 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = yr_re_node_create(RE_NODE_ANCHOR_END, NULL, NULL);
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1646 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 22:
#line 357 "re_grammar.y" /* yacc.c:1646  */
     {
        incr_ast_levels();

         (yyval.re_node) = (yyvsp[-1].re_node);
       }
#line 1656 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 23:
#line 363 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = yr_re_node_create(RE_NODE_ANY, NULL, NULL);
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1666 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 24:
#line 369 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = yr_re_node_create(RE_NODE_LITERAL, NULL, NULL);
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
 
         (yyval.re_node)->value = (yyvsp[0].integer);
       }
#line 1678 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 25:
#line 377 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = yr_re_node_create(RE_NODE_WORD_CHAR, NULL, NULL);
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1688 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 26:
#line 383 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = yr_re_node_create(RE_NODE_NON_WORD_CHAR, NULL, NULL);
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1698 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 27:
#line 389 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = yr_re_node_create(RE_NODE_SPACE, NULL, NULL);
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1708 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 28:
#line 395 "re_grammar.y" /* yacc.c:1646  */
     {
          (yyval.re_node) = yr_re_node_create(RE_NODE_NON_SPACE, NULL, NULL);
 
          ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1718 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 29:
#line 401 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = yr_re_node_create(RE_NODE_DIGIT, NULL, NULL);
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1728 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 30:
#line 407 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = yr_re_node_create(RE_NODE_NON_DIGIT, NULL, NULL);
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
       }
#line 1738 "re_grammar.c" /* yacc.c:1646  */
     break;
 
   case 31:
#line 413 "re_grammar.y" /* yacc.c:1646  */
     {
         (yyval.re_node) = yr_re_node_create(RE_NODE_CLASS, NULL, NULL);
 
         ERROR_IF((yyval.re_node) == NULL, ERROR_INSUFFICIENT_MEMORY);
 
         (yyval.re_node)->class_vector = (yyvsp[0].class_vector);
       }
#line 1750 "re_grammar.c" /* yacc.c:1646  */
     break;
 
 
#line 1754 "re_grammar.c" /* yacc.c:1646  */
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
      yyerror (yyscanner, lex_env, YY_("syntax error"));
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
        yyerror (yyscanner, lex_env, yymsgp);
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
                      yytoken, &yylval, yyscanner, lex_env);
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
                  yystos[yystate], yyvsp, yyscanner, lex_env);
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
  yyerror (yyscanner, lex_env, YY_("memory exhausted"));
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
                  yytoken, &yylval, yyscanner, lex_env);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yyscanner, lex_env);
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
