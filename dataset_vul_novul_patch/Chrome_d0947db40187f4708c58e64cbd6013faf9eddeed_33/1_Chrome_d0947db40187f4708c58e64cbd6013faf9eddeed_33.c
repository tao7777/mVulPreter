xmlParseStartTag2(xmlParserCtxtPtr ctxt, const xmlChar **pref,
                  const xmlChar **URI, int *tlen) {
    const xmlChar *localname;
    const xmlChar *prefix;
    const xmlChar *attname;
    const xmlChar *aprefix;
    const xmlChar *nsname;
    xmlChar *attvalue;
    const xmlChar **atts = ctxt->atts;
    int maxatts = ctxt->maxatts;
    int nratts, nbatts, nbdef;
    int i, j, nbNs, attval, oldline, oldcol;
    const xmlChar *base;
    unsigned long cur;
    int nsNr = ctxt->nsNr;

    if (RAW != '<') return(NULL);
    NEXT1;

    /*
     * NOTE: it is crucial with the SAX2 API to never call SHRINK beyond that
     *       point since the attribute values may be stored as pointers to
     *       the buffer and calling SHRINK would destroy them !
     *       The Shrinking is only possible once the full set of attribute
     *       callbacks have been done.
     */
reparse:
    SHRINK;
    base = ctxt->input->base;
    cur = ctxt->input->cur - ctxt->input->base;
    oldline = ctxt->input->line;
    oldcol = ctxt->input->col;
    nbatts = 0;
    nratts = 0;
    nbdef = 0;
    nbNs = 0;
    attval = 0;
    /* Forget any namespaces added during an earlier parse of this element. */
    ctxt->nsNr = nsNr;

    localname = xmlParseQName(ctxt, &prefix);
    if (localname == NULL) {
	xmlFatalErrMsg(ctxt, XML_ERR_NAME_REQUIRED,
		       "StartTag: invalid element name\n");
        return(NULL);
    }
    *tlen = ctxt->input->cur - ctxt->input->base - cur;

    /*
     * Now parse the attributes, it ends up with the ending
     *
     * (S Attribute)* S?
     */
    SKIP_BLANKS;
     GROW;
     if (ctxt->input->base != base) goto base_changed;
 
    while ((RAW != '>') && 
 	   ((RAW != '/') || (NXT(1) != '>')) &&
	   (IS_BYTE_CHAR(RAW))) {
 	const xmlChar *q = CUR_PTR;
 	unsigned int cons = ctxt->input->consumed;
 	int len = -1, alloc = 0;

	attname = xmlParseAttribute2(ctxt, prefix, localname,
	                             &aprefix, &attvalue, &len, &alloc);
	if (ctxt->input->base != base) {
	    if ((attvalue != NULL) && (alloc != 0))
	        xmlFree(attvalue);
	    attvalue = NULL;
	    goto base_changed;
	}
        if ((attname != NULL) && (attvalue != NULL)) {
	    if (len < 0) len = xmlStrlen(attvalue);
            if ((attname == ctxt->str_xmlns) && (aprefix == NULL)) {
	        const xmlChar *URL = xmlDictLookup(ctxt->dict, attvalue, len);
		xmlURIPtr uri;

                if (*URL != 0) {
		    uri = xmlParseURI((const char *) URL);
		    if (uri == NULL) {
			xmlNsErr(ctxt, XML_WAR_NS_URI,
			         "xmlns: '%s' is not a valid URI\n",
					   URL, NULL, NULL);
		    } else {
			if (uri->scheme == NULL) {
			    xmlNsWarn(ctxt, XML_WAR_NS_URI_RELATIVE,
				      "xmlns: URI %s is not absolute\n",
				      URL, NULL, NULL);
			}
			xmlFreeURI(uri);
		    }
		    if (URL == ctxt->str_xml_ns) {
			if (attname != ctxt->str_xml) {
			    xmlNsErr(ctxt, XML_NS_ERR_XML_NAMESPACE,
			 "xml namespace URI cannot be the default namespace\n",
				     NULL, NULL, NULL);
			}
			goto skip_default_ns;
		    }
		    if ((len == 29) &&
			(xmlStrEqual(URL,
				 BAD_CAST "http://www.w3.org/2000/xmlns/"))) {
			xmlNsErr(ctxt, XML_NS_ERR_XML_NAMESPACE,
			     "reuse of the xmlns namespace name is forbidden\n",
				 NULL, NULL, NULL);
			goto skip_default_ns;
		    }
		}
		/*
		 * check that it's not a defined namespace
		 */
		for (j = 1;j <= nbNs;j++)
		    if (ctxt->nsTab[ctxt->nsNr - 2 * j] == NULL)
			break;
		if (j <= nbNs)
		    xmlErrAttributeDup(ctxt, NULL, attname);
		else
		    if (nsPush(ctxt, NULL, URL) > 0) nbNs++;
skip_default_ns:
		if (alloc != 0) xmlFree(attvalue);
		SKIP_BLANKS;
		continue;
	    }
            if (aprefix == ctxt->str_xmlns) {
	        const xmlChar *URL = xmlDictLookup(ctxt->dict, attvalue, len);
		xmlURIPtr uri;

                if (attname == ctxt->str_xml) {
		    if (URL != ctxt->str_xml_ns) {
		        xmlNsErr(ctxt, XML_NS_ERR_XML_NAMESPACE,
			         "xml namespace prefix mapped to wrong URI\n",
			         NULL, NULL, NULL);
		    }
		    /*
		     * Do not keep a namespace definition node
		     */
		    goto skip_ns;
		}
                if (URL == ctxt->str_xml_ns) {
		    if (attname != ctxt->str_xml) {
		        xmlNsErr(ctxt, XML_NS_ERR_XML_NAMESPACE,
			         "xml namespace URI mapped to wrong prefix\n",
			         NULL, NULL, NULL);
		    }
		    goto skip_ns;
		}
                if (attname == ctxt->str_xmlns) {
		    xmlNsErr(ctxt, XML_NS_ERR_XML_NAMESPACE,
			     "redefinition of the xmlns prefix is forbidden\n",
			     NULL, NULL, NULL);
		    goto skip_ns;
		}
		if ((len == 29) &&
		    (xmlStrEqual(URL,
		                 BAD_CAST "http://www.w3.org/2000/xmlns/"))) {
		    xmlNsErr(ctxt, XML_NS_ERR_XML_NAMESPACE,
			     "reuse of the xmlns namespace name is forbidden\n",
			     NULL, NULL, NULL);
		    goto skip_ns;
		}
		if ((URL == NULL) || (URL[0] == 0)) {
		    xmlNsErr(ctxt, XML_NS_ERR_XML_NAMESPACE,
		             "xmlns:%s: Empty XML namespace is not allowed\n",
			          attname, NULL, NULL);
		    goto skip_ns;
		} else {
		    uri = xmlParseURI((const char *) URL);
		    if (uri == NULL) {
			xmlNsErr(ctxt, XML_WAR_NS_URI,
			     "xmlns:%s: '%s' is not a valid URI\n",
					   attname, URL, NULL);
		    } else {
			if ((ctxt->pedantic) && (uri->scheme == NULL)) {
			    xmlNsWarn(ctxt, XML_WAR_NS_URI_RELATIVE,
				      "xmlns:%s: URI %s is not absolute\n",
				      attname, URL, NULL);
			}
			xmlFreeURI(uri);
		    }
		}

		/*
		 * check that it's not a defined namespace
		 */
		for (j = 1;j <= nbNs;j++)
		    if (ctxt->nsTab[ctxt->nsNr - 2 * j] == attname)
			break;
		if (j <= nbNs)
		    xmlErrAttributeDup(ctxt, aprefix, attname);
		else
		    if (nsPush(ctxt, attname, URL) > 0) nbNs++;
skip_ns:
		if (alloc != 0) xmlFree(attvalue);
		SKIP_BLANKS;
		if (ctxt->input->base != base) goto base_changed;
		continue;
	    }

	    /*
	     * Add the pair to atts
	     */
	    if ((atts == NULL) || (nbatts + 5 > maxatts)) {
	        if (xmlCtxtGrowAttrs(ctxt, nbatts + 5) < 0) {
		    if (attvalue[len] == 0)
			xmlFree(attvalue);
		    goto failed;
		}
	        maxatts = ctxt->maxatts;
		atts = ctxt->atts;
	    }
	    ctxt->attallocs[nratts++] = alloc;
	    atts[nbatts++] = attname;
	    atts[nbatts++] = aprefix;
	    atts[nbatts++] = NULL; /* the URI will be fetched later */
	    atts[nbatts++] = attvalue;
	    attvalue += len;
	    atts[nbatts++] = attvalue;
	    /*
	     * tag if some deallocation is needed
	     */
	    if (alloc != 0) attval = 1;
	} else {
	    if ((attvalue != NULL) && (attvalue[len] == 0))
		xmlFree(attvalue);
	}

 failed:
 
 	GROW
 	if (ctxt->input->base != base) goto base_changed;
 	if ((RAW == '>') || (((RAW == '/') && (NXT(1) == '>'))))
 	    break;
	if (!IS_BLANK_CH(RAW)) {
	    xmlFatalErrMsg(ctxt, XML_ERR_SPACE_REQUIRED,
			   "attributes construct error\n");
	    break;
	}
	SKIP_BLANKS;
        if ((cons == ctxt->input->consumed) && (q == CUR_PTR) &&
            (attname == NULL) && (attvalue == NULL)) {
	    xmlFatalErr(ctxt, XML_ERR_INTERNAL_ERROR,
	         "xmlParseStartTag: problem parsing attributes\n");
	    break;
	}
        GROW;
	if (ctxt->input->base != base) goto base_changed;
    }

    /*
     * The attributes defaulting
     */
    if (ctxt->attsDefault != NULL) {
        xmlDefAttrsPtr defaults;

	defaults = xmlHashLookup2(ctxt->attsDefault, localname, prefix);
	if (defaults != NULL) {
	    for (i = 0;i < defaults->nbAttrs;i++) {
	        attname = defaults->values[5 * i];
		aprefix = defaults->values[5 * i + 1];

                /*
		 * special work for namespaces defaulted defs
		 */
		if ((attname == ctxt->str_xmlns) && (aprefix == NULL)) {
		    /*
		     * check that it's not a defined namespace
		     */
		    for (j = 1;j <= nbNs;j++)
		        if (ctxt->nsTab[ctxt->nsNr - 2 * j] == NULL)
			    break;
	            if (j <= nbNs) continue;

		    nsname = xmlGetNamespace(ctxt, NULL);
		    if (nsname != defaults->values[5 * i + 2]) {
			if (nsPush(ctxt, NULL,
			           defaults->values[5 * i + 2]) > 0)
			    nbNs++;
		    }
		} else if (aprefix == ctxt->str_xmlns) {
		    /*
		     * check that it's not a defined namespace
		     */
		    for (j = 1;j <= nbNs;j++)
		        if (ctxt->nsTab[ctxt->nsNr - 2 * j] == attname)
			    break;
	            if (j <= nbNs) continue;

		    nsname = xmlGetNamespace(ctxt, attname);
		    if (nsname != defaults->values[2]) {
			if (nsPush(ctxt, attname,
			           defaults->values[5 * i + 2]) > 0)
			    nbNs++;
		    }
		} else {
		    /*
		     * check that it's not a defined attribute
		     */
		    for (j = 0;j < nbatts;j+=5) {
			if ((attname == atts[j]) && (aprefix == atts[j+1]))
			    break;
		    }
		    if (j < nbatts) continue;

		    if ((atts == NULL) || (nbatts + 5 > maxatts)) {
			if (xmlCtxtGrowAttrs(ctxt, nbatts + 5) < 0) {
			    return(NULL);
			}
			maxatts = ctxt->maxatts;
			atts = ctxt->atts;
		    }
		    atts[nbatts++] = attname;
		    atts[nbatts++] = aprefix;
		    if (aprefix == NULL)
			atts[nbatts++] = NULL;
		    else
		        atts[nbatts++] = xmlGetNamespace(ctxt, aprefix);
		    atts[nbatts++] = defaults->values[5 * i + 2];
		    atts[nbatts++] = defaults->values[5 * i + 3];
		    if ((ctxt->standalone == 1) &&
		        (defaults->values[5 * i + 4] != NULL)) {
			xmlValidityError(ctxt, XML_DTD_STANDALONE_DEFAULTED, 
	  "standalone: attribute %s on %s defaulted from external subset\n",
	                                 attname, localname);
		    }
		    nbdef++;
		}
	    }
	}
    }

    /*
     * The attributes checkings
     */
    for (i = 0; i < nbatts;i += 5) {
        /*
	* The default namespace does not apply to attribute names.
	*/
	if (atts[i + 1] != NULL) {
	    nsname = xmlGetNamespace(ctxt, atts[i + 1]);
	    if (nsname == NULL) {
		xmlNsErr(ctxt, XML_NS_ERR_UNDEFINED_NAMESPACE,
		    "Namespace prefix %s for %s on %s is not defined\n",
		    atts[i + 1], atts[i], localname);
	    }
	    atts[i + 2] = nsname;
	} else
	    nsname = NULL;
	/*
	 * [ WFC: Unique Att Spec ]
	 * No attribute name may appear more than once in the same
	 * start-tag or empty-element tag. 
	 * As extended by the Namespace in XML REC.
	 */
        for (j = 0; j < i;j += 5) {
	    if (atts[i] == atts[j]) {
	        if (atts[i+1] == atts[j+1]) {
		    xmlErrAttributeDup(ctxt, atts[i+1], atts[i]);
		    break;
		}
		if ((nsname != NULL) && (atts[j + 2] == nsname)) {
		    xmlNsErr(ctxt, XML_NS_ERR_ATTRIBUTE_REDEFINED,
			     "Namespaced Attribute %s in '%s' redefined\n",
			     atts[i], nsname, NULL);
		    break;
		}
	    }
	}
    }

    nsname = xmlGetNamespace(ctxt, prefix);
    if ((prefix != NULL) && (nsname == NULL)) {
	xmlNsErr(ctxt, XML_NS_ERR_UNDEFINED_NAMESPACE,
	         "Namespace prefix %s on %s is not defined\n",
		 prefix, localname, NULL);
    }
    *pref = prefix;
    *URI = nsname;

    /*
     * SAX: Start of Element !
     */
    if ((ctxt->sax != NULL) && (ctxt->sax->startElementNs != NULL) &&
	(!ctxt->disableSAX)) {
	if (nbNs > 0)
	    ctxt->sax->startElementNs(ctxt->userData, localname, prefix,
			  nsname, nbNs, &ctxt->nsTab[ctxt->nsNr - 2 * nbNs],
			  nbatts / 5, nbdef, atts);
	else
	    ctxt->sax->startElementNs(ctxt->userData, localname, prefix,
	                  nsname, 0, NULL, nbatts / 5, nbdef, atts);
    }

    /*
     * Free up attribute allocated strings if needed
     */
    if (attval != 0) {
	for (i = 3,j = 0; j < nratts;i += 5,j++)
	    if ((ctxt->attallocs[j] != 0) && (atts[i] != NULL))
	        xmlFree((xmlChar *) atts[i]);
    }

    return(localname);

base_changed:
    /*
     * the attribute strings are valid iif the base didn't changed
     */
    if (attval != 0) {
	for (i = 3,j = 0; j < nratts;i += 5,j++)
	    if ((ctxt->attallocs[j] != 0) && (atts[i] != NULL))
	        xmlFree((xmlChar *) atts[i]);
    }
    ctxt->input->cur = ctxt->input->base + cur;
    ctxt->input->line = oldline;
    ctxt->input->col = oldcol;
    if (ctxt->wellFormed == 1) {
	goto reparse;
    }
    return(NULL);
}
