xdr_krb5_principal(XDR *xdrs, krb5_principal *objp)
{
    int	    ret;
    char	    *p = NULL;
    krb5_principal  pr = NULL;
    static krb5_context context = NULL;

    /* using a static context here is ugly, but should work
       ok, and the other solutions are even uglier */

    if (!context &&
	kadm5_init_krb5_context(&context))
       return(FALSE);

    switch(xdrs->x_op) {
    case XDR_ENCODE:
	if (*objp) {
	     if((ret = krb5_unparse_name(context, *objp, &p)) != 0)
		  return FALSE;
	}
	if(!xdr_nullstring(xdrs, &p))
	    return FALSE;
	if (p) free(p);
	break;
    case XDR_DECODE:
	if(!xdr_nullstring(xdrs, &p))
	    return FALSE;
	if (p) {
	     ret = krb5_parse_name(context, p, &pr);
	     if(ret != 0)
		  return FALSE;
	     *objp = pr;
	     free(p);
	} else
	     *objp = NULL;
	break;
     case XDR_FREE:
 	if(*objp != NULL)
 	    krb5_free_principal(context, *objp);
	*objp = NULL;
 	break;
     }
     return TRUE;
}
