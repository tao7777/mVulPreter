yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *yyscanner, HEX_LEX_ENVIRONMENT *lex_env)
{
  YYUSE (yyvaluep);
  YYUSE (yyscanner);
  YYUSE (lex_env);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
   switch (yytype)
     {
           case 16: /* tokens  */
#line 94 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1023 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
     case 17: /* token_sequence  */
#line 95 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1029 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
     case 18: /* token_or_range  */
#line 96 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1035 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
     case 19: /* token  */
#line 97 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1041 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
     case 21: /* range  */
#line 100 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1047 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
     case 22: /* alternatives  */
#line 99 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1053 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
     case 23: /* byte  */
#line 98 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1059 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
 
      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}
