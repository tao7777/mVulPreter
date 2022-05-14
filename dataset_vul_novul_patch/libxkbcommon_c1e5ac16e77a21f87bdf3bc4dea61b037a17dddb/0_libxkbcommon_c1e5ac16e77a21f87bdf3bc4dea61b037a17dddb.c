ExprAppendMultiKeysymList(ExprDef *expr, ExprDef *append)
{
    unsigned nSyms = darray_size(expr->keysym_list.syms);
    unsigned numEntries = darray_size(append->keysym_list.syms);

    darray_append(expr->keysym_list.symsMapIndex, nSyms);
     darray_append(expr->keysym_list.symsNumEntries, numEntries);
     darray_concat(expr->keysym_list.syms, append->keysym_list.syms);
 
    FreeStmt((ParseCommon *) append);
 
     return expr;
 }
