yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *yyscanner, RE_LEX_ENVIRONMENT *lex_env)
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
           case 6: /* _CLASS_  */
#line 96 "re_grammar.y" /* yacc.c:1257  */
       { yr_free(((*yyvaluep).class_vector)); }
#line 1045 "re_grammar.c" /* yacc.c:1257  */
         break;
 
     case 26: /* alternative  */
#line 97 "re_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1051 "re_grammar.c" /* yacc.c:1257  */
         break;
 
     case 27: /* concatenation  */
#line 98 "re_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1057 "re_grammar.c" /* yacc.c:1257  */
         break;
 
     case 28: /* repeat  */
#line 99 "re_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1063 "re_grammar.c" /* yacc.c:1257  */
         break;
 
     case 29: /* single  */
#line 100 "re_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1069 "re_grammar.c" /* yacc.c:1257  */
         break;
 
 
      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}
