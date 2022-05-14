 exsltCryptoRc4EncryptFunction (xmlXPathParserContextPtr ctxt, int nargs) {
 
    int key_len = 0, key_size = 0;
     int str_len = 0, bin_len = 0, hex_len = 0;
     xmlChar *key = NULL, *str = NULL, *padkey = NULL;
     xmlChar *bin = NULL, *hex = NULL;
    xsltTransformContextPtr tctxt = NULL;

    if (nargs != 2) {
	xmlXPathSetArityError (ctxt);
	return;
    }
     tctxt = xsltXPathGetTransformContext(ctxt);
 
     str = xmlXPathPopString (ctxt);
    str_len = xmlUTF8Strlen (str);
 
     if (str_len == 0) {
 	xmlXPathReturnEmptyString (ctxt);
	xmlFree (str);
	return;
     }
 
     key = xmlXPathPopString (ctxt);
    key_len = xmlUTF8Strlen (key);
 
     if (key_len == 0) {
 	xmlXPathReturnEmptyString (ctxt);
	xmlFree (key);
	xmlFree (str);
	return;
    }

    padkey = xmlMallocAtomic (RC4_KEY_LENGTH + 1);
    if (padkey == NULL) {
	xsltTransformError(tctxt, NULL, tctxt->inst,
	    "exsltCryptoRc4EncryptFunction: Failed to allocate padkey\n");
	tctxt->state = XSLT_STATE_STOPPED;
	xmlXPathReturnEmptyString (ctxt);
	goto done;
     }
     memset(padkey, 0, RC4_KEY_LENGTH + 1);
 
    key_size = xmlUTF8Strsize (key, key_len);
    if ((key_size > RC4_KEY_LENGTH) || (key_size < 0)) {
 	xsltTransformError(tctxt, NULL, tctxt->inst,
 	    "exsltCryptoRc4EncryptFunction: key size too long or key broken\n");
 	tctxt->state = XSLT_STATE_STOPPED;
 	xmlXPathReturnEmptyString (ctxt);
 	goto done;
     }
    memcpy (padkey, key, key_size);
 
 /* encrypt it */
     bin_len = str_len;
    bin = xmlStrdup (str);
    if (bin == NULL) {
	xsltTransformError(tctxt, NULL, tctxt->inst,
	    "exsltCryptoRc4EncryptFunction: Failed to allocate string\n");
	tctxt->state = XSLT_STATE_STOPPED;
	xmlXPathReturnEmptyString (ctxt);
	goto done;
    }
    PLATFORM_RC4_ENCRYPT (ctxt, padkey, str, str_len, bin, bin_len);

/* encode it */
    hex_len = str_len * 2 + 1;
    hex = xmlMallocAtomic (hex_len);
    if (hex == NULL) {
	xsltTransformError(tctxt, NULL, tctxt->inst,
	    "exsltCryptoRc4EncryptFunction: Failed to allocate result\n");
	tctxt->state = XSLT_STATE_STOPPED;
	xmlXPathReturnEmptyString (ctxt);
	goto done;
    }

    exsltCryptoBin2Hex (bin, str_len, hex, hex_len);
    xmlXPathReturnString (ctxt, hex);

done:
    if (key != NULL)
	xmlFree (key);
    if (str != NULL)
	xmlFree (str);
    if (padkey != NULL)
	xmlFree (padkey);
    if (bin != NULL)
	xmlFree (bin);
}
