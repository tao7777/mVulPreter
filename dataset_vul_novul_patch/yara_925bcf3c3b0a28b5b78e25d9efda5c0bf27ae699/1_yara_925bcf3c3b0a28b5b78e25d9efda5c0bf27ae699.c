int yr_re_ast_create(
    RE_AST** re_ast)
{
  *re_ast = (RE_AST*) yr_malloc(sizeof(RE_AST));

  if (*re_ast == NULL)
     return ERROR_INSUFFICIENT_MEMORY;
 
   (*re_ast)->flags = 0;
   (*re_ast)->root_node = NULL;
 
   return ERROR_SUCCESS;
}
