exsltStrAlignFunction (xmlXPathParserContextPtr ctxt, int nargs) {
    xmlChar *str, *padding, *alignment, *ret;
    int str_l, padding_l;

    if ((nargs < 2) || (nargs > 3)) {
	xmlXPathSetArityError(ctxt);
	return;
    }

    if (nargs == 3)
	alignment = xmlXPathPopString(ctxt);
    else
	alignment = NULL;

    padding = xmlXPathPopString(ctxt);
    str = xmlXPathPopString(ctxt);

    str_l = xmlUTF8Strlen (str);
    padding_l = xmlUTF8Strlen (padding);

    if (str_l == padding_l) {
	xmlXPathReturnString (ctxt, str);
	xmlFree(padding);
	xmlFree(alignment);
	return;
    }

    if (str_l > padding_l) {
	ret = xmlUTF8Strndup (str, padding_l);
    } else {
	if (xmlStrEqual(alignment, (const xmlChar *) "right")) {
	    ret = xmlUTF8Strndup (padding, padding_l - str_l);
	    ret = xmlStrcat (ret, str);
	} else if (xmlStrEqual(alignment, (const xmlChar *) "center")) {
	    int left = (padding_l - str_l) / 2;
	    int right_start;

	    ret = xmlUTF8Strndup (padding, left);
	    ret = xmlStrcat (ret, str);

	    right_start = xmlUTF8Strsize (padding, left + str_l);
	    ret = xmlStrcat (ret, padding + right_start);
 	} else {
 	    int str_s;
 
	    str_s = xmlStrlen (str);
 	    ret = xmlStrdup (str);
 	    ret = xmlStrcat (ret, padding + str_s);
 	}
    }

    xmlXPathReturnString (ctxt, ret);

    xmlFree(str);
    xmlFree(padding);
    xmlFree(alignment);
}
