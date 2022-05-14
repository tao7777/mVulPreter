exsltDateCreateDate (exsltDateType type)
{
    exsltDateValPtr ret;

    ret = (exsltDateValPtr) xmlMalloc(sizeof(exsltDateVal));
    if (ret == NULL) {
	xsltGenericError(xsltGenericErrorContext,
			 "exsltDateCreateDate: out of memory\n");
	return (NULL);
     }
     memset (ret, 0, sizeof(exsltDateVal));
 
     if (type != EXSLT_UNKNOWN)
         ret->type = type;
 
    return ret;
}
