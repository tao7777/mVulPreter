PHPAPI int php_var_unserialize(UNSERIALIZE_PARAMETER)
{
	const unsigned char *cursor, *limit, *marker, *start;
	zval **rval_ref;

	limit = max;
	cursor = *p;
	
	if (YYCURSOR >= YYLIMIT) {
		return 0;
	}
	
	if (var_hash && cursor[0] != 'R') {
		var_push(var_hash, rval);
	}

	start = cursor;

	
	

        
        
 
#line 487 "ext/standard/var_unserializer.c"
 {
        YYCTYPE yych;
        static const unsigned char yybm[] = {
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		128, 128, 128, 128, 128, 128, 128, 128, 
		128, 128,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
		  0,   0,   0,   0,   0,   0,   0,   0, 
	};

	if ((YYLIMIT - YYCURSOR) < 7) YYFILL(7);
	yych = *YYCURSOR;
	switch (yych) {
	case 'C':
	case 'O':	goto yy13;
	case 'N':	goto yy5;
	case 'R':	goto yy2;
	case 'S':	goto yy10;
	case 'a':	goto yy11;
	case 'b':	goto yy6;
	case 'd':	goto yy8;
	case 'i':	goto yy7;
	case 'o':	goto yy12;
	case 'r':	goto yy4;
	case 's':	goto yy9;
	case '}':	goto yy14;
	default:	goto yy16;
	}
yy2:
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych == ':') goto yy95;
yy3:
        yych = *(YYMARKER = ++YYCURSOR);
        if (yych == ':') goto yy95;
 yy3:
#line 838 "ext/standard/var_unserializer.re"
        { return 0; }
#line 549 "ext/standard/var_unserializer.c"
 yy4:
        yych = *(YYMARKER = ++YYCURSOR);
        if (yych == ':') goto yy89;
	if (yych == ';') goto yy87;
	goto yy3;
yy6:
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych == ':') goto yy83;
	goto yy3;
yy7:
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych == ':') goto yy77;
	goto yy3;
yy8:
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych == ':') goto yy53;
	goto yy3;
yy9:
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych == ':') goto yy46;
	goto yy3;
yy10:
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych == ':') goto yy39;
	goto yy3;
yy11:
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych == ':') goto yy32;
	goto yy3;
yy12:
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych == ':') goto yy25;
	goto yy3;
yy13:
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych == ':') goto yy17;
	goto yy3;
yy14:
	++YYCURSOR;
        goto yy3;
 yy14:
        ++YYCURSOR;
#line 832 "ext/standard/var_unserializer.re"
        {
        /* this is the case where we have less data than planned */
        php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unexpected end of serialized data");
        return 0; /* not sure if it should be 0 or 1 here? */
 }
#line 598 "ext/standard/var_unserializer.c"
 yy16:
        yych = *++YYCURSOR;
        goto yy3;
		goto yy20;
	}
