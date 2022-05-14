 exsltCryptoPopString (xmlXPathParserContextPtr ctxt, int nargs,
		      xmlChar ** str) {

    int str_len = 0;

    if ((nargs < 1) || (nargs > 2)) {
	xmlXPathSetArityError (ctxt);
	return 0;
     }
 
     *str = xmlXPathPopString (ctxt);
    str_len = xmlUTF8Strlen (*str);
 
     if (str_len == 0) {
 	xmlXPathReturnEmptyString (ctxt);
	xmlFree (*str);
	return 0;
    }

    return str_len;
}
