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
#line 101 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1030 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
     case 17: /* token_sequence  */
#line 102 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1036 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
     case 18: /* token_or_range  */
#line 103 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1042 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
     case 19: /* token  */
#line 104 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1048 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
     case 21: /* range  */
#line 107 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1054 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
     case 22: /* alternatives  */
#line 106 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1060 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
     case 23: /* byte  */
#line 105 "hex_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1066 "hex_grammar.c" /* yacc.c:1257  */
         break;
 
 
      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}
