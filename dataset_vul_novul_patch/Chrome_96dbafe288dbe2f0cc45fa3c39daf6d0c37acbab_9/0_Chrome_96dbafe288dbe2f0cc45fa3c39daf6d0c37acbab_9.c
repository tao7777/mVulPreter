 exsltStrPaddingFunction (xmlXPathParserContextPtr ctxt, int nargs) {
    int number, str_len = 0, str_size = 0;
    xmlChar *str = NULL, *ret = NULL;
 
     if ((nargs < 1) || (nargs > 2)) {
 	xmlXPathSetArityError(ctxt);
	return;
    }

     if (nargs == 2) {
 	str = xmlXPathPopString(ctxt);
 	str_len = xmlUTF8Strlen(str);
	str_size = xmlStrlen(str);
     }
     if (str_len == 0) {
 	if (str != NULL) xmlFree(str);
 	str = xmlStrdup((const xmlChar *) " ");
 	str_len = 1;
	str_size = 1;
     }
 
     number = (int) xmlXPathPopNumber(ctxt);

    if (number <= 0) {
	xmlXPathReturnEmptyString(ctxt);
	xmlFree(str);
	return;
     }
 
     while (number >= str_len) {
	ret = xmlStrncat(ret, str, str_size);
 	number -= str_len;
     }
    if (number > 0) {
	str_size = xmlUTF8Strsize(str, number);
	ret = xmlStrncat(ret, str, str_size);
    }
 
     xmlXPathReturnString(ctxt, ret);
 
    if (str != NULL)
	xmlFree(str);
}
