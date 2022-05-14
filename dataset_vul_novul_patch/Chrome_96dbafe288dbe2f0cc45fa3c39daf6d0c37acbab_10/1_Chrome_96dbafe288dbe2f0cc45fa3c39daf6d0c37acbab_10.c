exsltStrXpathCtxtRegister (xmlXPathContextPtr ctxt, const xmlChar *prefix)
{
    if (ctxt
        && prefix
        && !xmlXPathRegisterNs(ctxt,
                               prefix,
                               (const xmlChar *) EXSLT_STRINGS_NAMESPACE)
        && !xmlXPathRegisterFuncNS(ctxt,
                                   (const xmlChar *) "encode-uri",
                                   (const xmlChar *) EXSLT_STRINGS_NAMESPACE,
                                   exsltStrEncodeUriFunction)
        && !xmlXPathRegisterFuncNS(ctxt,
                                   (const xmlChar *) "decode-uri",
                                   (const xmlChar *) EXSLT_STRINGS_NAMESPACE,
                                   exsltStrDecodeUriFunction)
        && !xmlXPathRegisterFuncNS(ctxt,
                                   (const xmlChar *) "padding",
                                   (const xmlChar *) EXSLT_STRINGS_NAMESPACE,
                                   exsltStrPaddingFunction)
        && !xmlXPathRegisterFuncNS(ctxt,
                                   (const xmlChar *) "align",
                                   (const xmlChar *) EXSLT_STRINGS_NAMESPACE,
                                   exsltStrAlignFunction)
         && !xmlXPathRegisterFuncNS(ctxt,
                                    (const xmlChar *) "concat",
                                    (const xmlChar *) EXSLT_STRINGS_NAMESPACE,
                                   exsltStrConcatFunction)
        && !xmlXPathRegisterFuncNS(ctxt,
                                   (const xmlChar *) "replace",
                                   (const xmlChar *) EXSLT_STRINGS_NAMESPACE,
                                   exsltStrReplaceFunction)) {
         return 0;
     }
     return -1;
}
