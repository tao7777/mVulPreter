 exsltStrPaddingFunction (xmlXPathParserContextPtr ctxt, int nargs) {
    int number, str_len = 0;
    xmlChar *str = NULL, *ret = NULL, *tmp;
 
     if ((nargs < 1) || (nargs > 2)) {
 	xmlXPathSetArityError(ctxt);
	return;
    }

     if (nargs == 2) {
 	str = xmlXPathPopString(ctxt);
 	str_len = xmlUTF8Strlen(str);
     }
     if (str_len == 0) {
 	if (str != NULL) xmlFree(str);
 	str = xmlStrdup((const xmlChar *) " ");
 	str_len = 1;
     }
 
     number = (int) xmlXPathPopNumber(ctxt);

    if (number <= 0) {
	xmlXPathReturnEmptyString(ctxt);
	xmlFree(str);
	return;
     }
 
     while (number >= str_len) {
	ret = xmlStrncat(ret, str, str_len);
 	number -= str_len;
     }
    tmp = xmlUTF8Strndup (str, number);
    ret = xmlStrcat(ret, tmp);
    if (tmp != NULL)
	xmlFree (tmp);
 
     xmlXPathReturnString(ctxt, ret);
 
    if (str != NULL)
	xmlFree(str);
}
