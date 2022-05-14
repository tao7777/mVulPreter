yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
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
  int yytoken;
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

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
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
  *++yyvsp = yylval;

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
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:

/* Line 1455 of yacc.c  */
#line 320 "ntp_parser.y"
    {
			/* I will need to incorporate much more fine grained
			 * error messages. The following should suffice for
			 * the time being.
			 */
			msyslog(LOG_ERR, 
				"syntax error in %s line %d, column %d",
				ip_file->fname,
				ip_file->err_line_no,
				ip_file->err_col_no);
		}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 354 "ntp_parser.y"
    {
			struct peer_node *my_node =  create_peer_node((yyvsp[(1) - (3)].Integer), (yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue));
			if (my_node)
				enqueue(cfgt.peers, my_node);
		}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 360 "ntp_parser.y"
    {
			struct peer_node *my_node = create_peer_node((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node), NULL);
			if (my_node)
				enqueue(cfgt.peers, my_node);
		}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 377 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 378 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(2) - (2)].String), AF_INET6); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 382 "ntp_parser.y"
    { (yyval.Address_node) = create_address_node((yyvsp[(1) - (1)].String), 0); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 386 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 387 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 391 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 392 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 393 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 394 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 395 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 396 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 397 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 398 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 399 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 400 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 401 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 402 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 403 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 404 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 405 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 415 "ntp_parser.y"
    {
			struct unpeer_node *my_node = create_unpeer_node((yyvsp[(2) - (2)].Address_node));
			if (my_node)
				enqueue(cfgt.unpeers, my_node);
		}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 434 "ntp_parser.y"
    { cfgt.broadcastclient = 1; }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 436 "ntp_parser.y"
    { append_queue(cfgt.manycastserver, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 438 "ntp_parser.y"
    { append_queue(cfgt.multicastclient, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 449 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer))); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 451 "ntp_parser.y"
    { cfgt.auth.control_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 453 "ntp_parser.y"
    { 
			cfgt.auth.cryptosw++;
			append_queue(cfgt.auth.crypto_cmd_list, (yyvsp[(2) - (2)].Queue));
		}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 458 "ntp_parser.y"
    { cfgt.auth.keys = (yyvsp[(2) - (2)].String); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 460 "ntp_parser.y"
    { cfgt.auth.keysdir = (yyvsp[(2) - (2)].String); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 462 "ntp_parser.y"
    { cfgt.auth.request_key = (yyvsp[(2) - (2)].Integer); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 464 "ntp_parser.y"
    { cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 466 "ntp_parser.y"
    { cfgt.auth.trusted_key_list = (yyvsp[(2) - (2)].Queue); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 468 "ntp_parser.y"
    { cfgt.auth.ntp_signd_socket = (yyvsp[(2) - (2)].String); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 474 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 479 "ntp_parser.y"
    { 
			if ((yyvsp[(2) - (2)].Attr_val) != NULL)
				(yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val));
			else
				(yyval.Queue) = (yyvsp[(1) - (2)].Queue);
		}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 486 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Attr_val) != NULL)
				(yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val));
			else
				(yyval.Queue) = create_queue();
		}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 496 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 498 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 500 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 502 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 504 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 506 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 508 "ntp_parser.y"
    {
			(yyval.Attr_val) = NULL;
			cfgt.auth.revoke = (yyvsp[(2) - (2)].Integer);
			msyslog(LOG_WARNING,
				"'crypto revoke %d' is deprecated, "
				"please use 'revoke %d' instead.",
				cfgt.auth.revoke, cfgt.auth.revoke);
		}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 525 "ntp_parser.y"
    { append_queue(cfgt.orphan_cmds,(yyvsp[(2) - (2)].Queue)); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 529 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 530 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 535 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 537 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 539 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 541 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 543 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 545 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 547 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 549 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 551 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 553 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 555 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (double)(yyvsp[(2) - (2)].Integer)); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 565 "ntp_parser.y"
    { append_queue(cfgt.stats_list, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 567 "ntp_parser.y"
    {
			if (input_from_file)
				cfgt.stats_dir = (yyvsp[(2) - (2)].String);
			else {
				free((yyvsp[(2) - (2)].String));
				yyerror("statsdir remote configuration ignored");
			}
		}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 576 "ntp_parser.y"
    {
			enqueue(cfgt.filegen_opts,
				create_filegen_node((yyvsp[(2) - (3)].Integer), (yyvsp[(3) - (3)].Queue)));
		}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 583 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 584 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 600 "ntp_parser.y"
    {
			if ((yyvsp[(2) - (2)].Attr_val) != NULL)
				(yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val));
			else
				(yyval.Queue) = (yyvsp[(1) - (2)].Queue);
		}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 607 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Attr_val) != NULL)
				(yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val));
			else
				(yyval.Queue) = create_queue();
		}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 617 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String));
			else {
				(yyval.Attr_val) = NULL;
				free((yyvsp[(2) - (2)].String));
				yyerror("filegen file remote configuration ignored");
			}
		}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 627 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen type remote configuration ignored");
			}
		}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 636 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen link remote configuration ignored");
			}
		}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 645 "ntp_parser.y"
    {
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("filegen nolink remote configuration ignored");
			}
		}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 653 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 654 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 674 "ntp_parser.y"
    {
			append_queue(cfgt.discard_opts, (yyvsp[(2) - (2)].Queue));
		}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 678 "ntp_parser.y"
    {
			append_queue(cfgt.mru_opts, (yyvsp[(2) - (2)].Queue));
		}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 682 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node((yyvsp[(2) - (3)].Address_node), NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 687 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node((yyvsp[(2) - (5)].Address_node), (yyvsp[(4) - (5)].Address_node), (yyvsp[(5) - (5)].Queue), ip_file->line_no));
		}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 692 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(NULL, NULL, (yyvsp[(3) - (3)].Queue), ip_file->line_no));
		}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 697 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(
					create_address_node(
						estrdup("0.0.0.0"), 
						AF_INET),
					create_address_node(
						estrdup("0.0.0.0"), 
						AF_INET),
					(yyvsp[(4) - (4)].Queue), 
					ip_file->line_no));
		}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 710 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(
					create_address_node(
						estrdup("::"), 
						AF_INET6),
					create_address_node(
						estrdup("::"), 
						AF_INET6),
					(yyvsp[(4) - (4)].Queue), 
					ip_file->line_no));
		}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 723 "ntp_parser.y"
    {
			enqueue(cfgt.restrict_opts,
				create_restrict_node(
					NULL, NULL,
					enqueue((yyvsp[(3) - (3)].Queue), create_ival((yyvsp[(2) - (3)].Integer))),
					ip_file->line_no));
		}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 734 "ntp_parser.y"
    { (yyval.Queue) = create_queue(); }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 736 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 758 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 760 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 764 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 765 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 766 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 771 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 773 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 777 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 778 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 779 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 780 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 781 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 782 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 783 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 784 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 793 "ntp_parser.y"
    { enqueue(cfgt.fudge, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); }
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 798 "ntp_parser.y"
    { enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 800 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 804 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 805 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 806 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 807 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)); }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 808 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 809 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 810 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 811 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 820 "ntp_parser.y"
    { append_queue(cfgt.enable_opts, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 822 "ntp_parser.y"
    { append_queue(cfgt.disable_opts, (yyvsp[(2) - (2)].Queue));  }
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 827 "ntp_parser.y"
    {
			if ((yyvsp[(2) - (2)].Attr_val) != NULL)
				(yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val));
			else
				(yyval.Queue) = (yyvsp[(1) - (2)].Queue);
		}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 834 "ntp_parser.y"
    {
			if ((yyvsp[(1) - (1)].Attr_val) != NULL)
				(yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val));
			else
				(yyval.Queue) = create_queue();
		}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 843 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 844 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 845 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 846 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 847 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 848 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer)); }
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 850 "ntp_parser.y"
    { 
			if (input_from_file)
				(yyval.Attr_val) = create_attr_ival(T_Flag, (yyvsp[(1) - (1)].Integer));
			else {
				(yyval.Attr_val) = NULL;
				yyerror("enable/disable stats remote configuration ignored");
			}
		}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 865 "ntp_parser.y"
    { append_queue(cfgt.tinker, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 869 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 870 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 874 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 875 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 876 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 877 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 878 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 879 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 880 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double)); }
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 891 "ntp_parser.y"
    {
			if (curr_include_level >= MAXINCLUDELEVEL) {
				fprintf(stderr, "getconfig: Maximum include file level exceeded.\n");
				msyslog(LOG_ERR, "getconfig: Maximum include file level exceeded.");
			}
			else {
				fp[curr_include_level + 1] = F_OPEN(FindConfig((yyvsp[(2) - (3)].String)), "r");
				if (fp[curr_include_level + 1] == NULL) {
					fprintf(stderr, "getconfig: Couldn't open <%s>\n", FindConfig((yyvsp[(2) - (3)].String)));
					msyslog(LOG_ERR, "getconfig: Couldn't open <%s>", FindConfig((yyvsp[(2) - (3)].String)));
				}
				else
					ip_file = fp[++curr_include_level];
			}
		}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 907 "ntp_parser.y"
    {
			while (curr_include_level != -1)
				FCLOSE(fp[curr_include_level--]);
		}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 913 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double))); }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 915 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer))); }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 917 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Double))); }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 919 "ntp_parser.y"
    { /* Null action, possibly all null parms */ }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 921 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 924 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 926 "ntp_parser.y"
    {
			if (input_from_file)
				enqueue(cfgt.vars,
					create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)));
			else {
				free((yyvsp[(2) - (2)].String));
				yyerror("logfile remote configuration ignored");
			}
		}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 937 "ntp_parser.y"
    { append_queue(cfgt.logconfig, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 939 "ntp_parser.y"
    { append_queue(cfgt.phone, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 941 "ntp_parser.y"
    {
			if (input_from_file)
				enqueue(cfgt.vars,
					create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String)));
			else {
				free((yyvsp[(2) - (2)].String));
				yyerror("saveconfigdir remote configuration ignored");
			}
		}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 951 "ntp_parser.y"
    { enqueue(cfgt.setvar, (yyvsp[(2) - (2)].Set_var)); }
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 953 "ntp_parser.y"
    { enqueue(cfgt.trap, create_addr_opts_node((yyvsp[(2) - (2)].Address_node), NULL)); }
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 955 "ntp_parser.y"
    { enqueue(cfgt.trap, create_addr_opts_node((yyvsp[(2) - (3)].Address_node), (yyvsp[(3) - (3)].Queue))); }
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 957 "ntp_parser.y"
    { append_queue(cfgt.ttl, (yyvsp[(2) - (2)].Queue)); }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 959 "ntp_parser.y"
    { enqueue(cfgt.qos, create_attr_sval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].String))); }
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 964 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (1)].String))); }
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 966 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_dval(T_WanderThreshold, (yyvsp[(2) - (2)].Double)));
			  enqueue(cfgt.vars, create_attr_sval(T_Driftfile, (yyvsp[(1) - (2)].String))); }
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 969 "ntp_parser.y"
    { enqueue(cfgt.vars, create_attr_sval(T_Driftfile, "\0")); }
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 974 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (4)].String), (yyvsp[(3) - (4)].String), (yyvsp[(4) - (4)].Integer)); }
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 976 "ntp_parser.y"
    { (yyval.Set_var) = create_setvar_node((yyvsp[(1) - (3)].String), (yyvsp[(3) - (3)].String), 0); }
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 981 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 982 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 986 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_ival((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Integer)); }
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 987 "ntp_parser.y"
    { (yyval.Attr_val) = create_attr_pval((yyvsp[(1) - (2)].Integer), (yyvsp[(2) - (2)].Address_node)); }
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 991 "ntp_parser.y"
    { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 992 "ntp_parser.y"
    { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
    break;

  case 215:

 /* Line 1455 of yacc.c  */
 #line 997 "ntp_parser.y"
     {
			char	prefix;
			char *	type;
 			
			switch ((yyvsp[(1) - (1)].String)[0]) {
			
			case '+':
			case '-':
			case '=':
				prefix = (yyvsp[(1) - (1)].String)[0];
				type = (yyvsp[(1) - (1)].String) + 1;
				break;
				
			default:
				prefix = '=';
				type = (yyvsp[(1) - (1)].String);
			}	
			
			(yyval.Attr_val) = create_attr_sval(prefix, estrdup(type));
 			YYFREE((yyvsp[(1) - (1)].String));
 		}
     break;
 
   case 216:
 
 /* Line 1455 of yacc.c  */
#line 1022 "ntp_parser.y"
     {
 			enqueue(cfgt.nic_rules,
 				create_nic_rule_node((yyvsp[(3) - (3)].Integer), NULL, (yyvsp[(2) - (3)].Integer)));
		}
    break;

   case 217:
 
 /* Line 1455 of yacc.c  */
#line 1027 "ntp_parser.y"
     {
 			enqueue(cfgt.nic_rules,
 				create_nic_rule_node(0, (yyvsp[(3) - (3)].String), (yyvsp[(2) - (3)].Integer)));
		}
    break;

   case 227:
 
 /* Line 1455 of yacc.c  */
#line 1058 "ntp_parser.y"
     { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_ival((yyvsp[(2) - (2)].Integer))); }
     break;
 
   case 228:
 
 /* Line 1455 of yacc.c  */
#line 1059 "ntp_parser.y"
     { (yyval.Queue) = enqueue_in_new_queue(create_ival((yyvsp[(1) - (1)].Integer))); }
     break;
 
   case 229:
 
 /* Line 1455 of yacc.c  */
#line 1064 "ntp_parser.y"
     { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Attr_val)); }
     break;
 
   case 230:
 
 /* Line 1455 of yacc.c  */
#line 1066 "ntp_parser.y"
     { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Attr_val)); }
     break;
 
   case 231:
 
 /* Line 1455 of yacc.c  */
#line 1071 "ntp_parser.y"
     { (yyval.Attr_val) = create_attr_ival('i', (yyvsp[(1) - (1)].Integer)); }
     break;
 
   case 233:
 
 /* Line 1455 of yacc.c  */
#line 1077 "ntp_parser.y"
     { (yyval.Attr_val) = create_attr_shorts('-', (yyvsp[(2) - (5)].Integer), (yyvsp[(4) - (5)].Integer)); }
     break;
 
   case 234:
 
 /* Line 1455 of yacc.c  */
#line 1081 "ntp_parser.y"
     { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), create_pval((yyvsp[(2) - (2)].String))); }
     break;
 
   case 235:
 
 /* Line 1455 of yacc.c  */
#line 1082 "ntp_parser.y"
     { (yyval.Queue) = enqueue_in_new_queue(create_pval((yyvsp[(1) - (1)].String))); }
     break;
 
   case 236:
 
 /* Line 1455 of yacc.c  */
#line 1086 "ntp_parser.y"
     { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Address_node)); }
     break;
 
   case 237:
 
 /* Line 1455 of yacc.c  */
#line 1087 "ntp_parser.y"
     { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Address_node)); }
     break;
 
   case 238:
 
 /* Line 1455 of yacc.c  */
#line 1092 "ntp_parser.y"
     {
 			if ((yyvsp[(1) - (1)].Integer) != 0 && (yyvsp[(1) - (1)].Integer) != 1) {
 				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				(yyval.Integer) = 1;
			}
			else
				(yyval.Integer) = (yyvsp[(1) - (1)].Integer);
		}
    break;

   case 239:
 
 /* Line 1455 of yacc.c  */
#line 1100 "ntp_parser.y"
     { (yyval.Integer) = 1; }
     break;
 
   case 240:
 
 /* Line 1455 of yacc.c  */
#line 1101 "ntp_parser.y"
     { (yyval.Integer) = 0; }
     break;
 
   case 241:
 
 /* Line 1455 of yacc.c  */
#line 1105 "ntp_parser.y"
     { (yyval.Double) = (double)(yyvsp[(1) - (1)].Integer); }
     break;
 
   case 243:
 
 /* Line 1455 of yacc.c  */
#line 1116 "ntp_parser.y"
     {
 			cfgt.sim_details = create_sim_node((yyvsp[(3) - (5)].Queue), (yyvsp[(4) - (5)].Queue));
 
			/* Reset the old_config_style variable */
			old_config_style = 1;
		}
    break;

   case 244:
 
 /* Line 1455 of yacc.c  */
#line 1130 "ntp_parser.y"
     { old_config_style = 0; }
     break;
 
   case 245:
 
 /* Line 1455 of yacc.c  */
#line 1134 "ntp_parser.y"
     { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
     break;
 
   case 246:
 
 /* Line 1455 of yacc.c  */
#line 1135 "ntp_parser.y"
     { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
     break;
 
   case 247:
 
 /* Line 1455 of yacc.c  */
#line 1139 "ntp_parser.y"
     { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
     break;
 
   case 248:
 
 /* Line 1455 of yacc.c  */
#line 1140 "ntp_parser.y"
     { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
     break;
 
   case 249:
 
 /* Line 1455 of yacc.c  */
#line 1144 "ntp_parser.y"
     { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_server)); }
     break;
 
   case 250:
 
 /* Line 1455 of yacc.c  */
#line 1145 "ntp_parser.y"
     { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_server)); }
     break;
 
   case 251:
 
 /* Line 1455 of yacc.c  */
#line 1150 "ntp_parser.y"
     { (yyval.Sim_server) = create_sim_server((yyvsp[(1) - (5)].Address_node), (yyvsp[(3) - (5)].Double), (yyvsp[(4) - (5)].Queue)); }
     break;
 
   case 252:
 
 /* Line 1455 of yacc.c  */
#line 1154 "ntp_parser.y"
     { (yyval.Double) = (yyvsp[(3) - (4)].Double); }
     break;
 
   case 253:
 
 /* Line 1455 of yacc.c  */
#line 1158 "ntp_parser.y"
     { (yyval.Address_node) = (yyvsp[(3) - (3)].Address_node); }
     break;
 
   case 254:
 
 /* Line 1455 of yacc.c  */
#line 1162 "ntp_parser.y"
     { (yyval.Queue) = enqueue((yyvsp[(1) - (2)].Queue), (yyvsp[(2) - (2)].Sim_script)); }
     break;
 
   case 255:
 
 /* Line 1455 of yacc.c  */
#line 1163 "ntp_parser.y"
     { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (1)].Sim_script)); }
     break;
 
   case 256:
 
 /* Line 1455 of yacc.c  */
#line 1168 "ntp_parser.y"
     { (yyval.Sim_script) = create_sim_script_info((yyvsp[(3) - (6)].Double), (yyvsp[(5) - (6)].Queue)); }
     break;
 
   case 257:
 
 /* Line 1455 of yacc.c  */
#line 1172 "ntp_parser.y"
     { (yyval.Queue) = enqueue((yyvsp[(1) - (3)].Queue), (yyvsp[(2) - (3)].Attr_val)); }
     break;
 
   case 258:
 
 /* Line 1455 of yacc.c  */
#line 1173 "ntp_parser.y"
     { (yyval.Queue) = enqueue_in_new_queue((yyvsp[(1) - (2)].Attr_val)); }
     break;
 
   case 259:
 
 /* Line 1455 of yacc.c  */
#line 1178 "ntp_parser.y"
     { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
     break;
 
   case 260:
 
 /* Line 1455 of yacc.c  */
#line 1180 "ntp_parser.y"
     { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
     break;
 
   case 261:
 
 /* Line 1455 of yacc.c  */
#line 1182 "ntp_parser.y"
     { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
     break;
 
   case 262:
 
 /* Line 1455 of yacc.c  */
#line 1184 "ntp_parser.y"
     { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
     break;
 
   case 263:
 
 /* Line 1455 of yacc.c  */
#line 1186 "ntp_parser.y"
     { (yyval.Attr_val) = create_attr_dval((yyvsp[(1) - (3)].Integer), (yyvsp[(3) - (3)].Double)); }
     break;
 
 
 
 /* Line 1455 of yacc.c  */
#line 3836 "ntp_parser.c"
       default: break;
     }
   YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
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
		      yytoken, &yylval);
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

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}
