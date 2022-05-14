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
#line 104 "re_grammar.y" /* yacc.c:1257  */
       { yr_free(((*yyvaluep).class_vector)); }
#line 1053 "re_grammar.c" /* yacc.c:1257  */
         break;
 
     case 26: /* alternative  */
#line 105 "re_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1059 "re_grammar.c" /* yacc.c:1257  */
         break;
 
     case 27: /* concatenation  */
#line 106 "re_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1065 "re_grammar.c" /* yacc.c:1257  */
         break;
 
     case 28: /* repeat  */
#line 107 "re_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1071 "re_grammar.c" /* yacc.c:1257  */
         break;
 
     case 29: /* single  */
#line 108 "re_grammar.y" /* yacc.c:1257  */
       { yr_re_node_destroy(((*yyvaluep).re_node)); }
#line 1077 "re_grammar.c" /* yacc.c:1257  */
         break;
 
 
      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}
