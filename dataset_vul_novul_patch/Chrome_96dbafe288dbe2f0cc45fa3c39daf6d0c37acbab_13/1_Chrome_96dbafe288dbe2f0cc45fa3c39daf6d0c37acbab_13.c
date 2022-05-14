xsltRegisterExtModuleElement(const xmlChar * name, const xmlChar * URI,
                              xsltPreComputeFunction precomp,
                              xsltTransformFunction transform)
 {
    int ret;
 
     xsltExtElementPtr ext;
 
    if ((name == NULL) || (URI == NULL) || (transform == NULL))
        return (-1);

    if (xsltElementsHash == NULL)
        xsltElementsHash = xmlHashCreate(10);
    if (xsltElementsHash == NULL)
        return (-1);

    xmlMutexLock(xsltExtMutex);

    ext = xsltNewExtElement(precomp, transform);
    if (ext == NULL) {
        ret = -1;
        goto done;
    }

    xmlHashUpdateEntry2(xsltElementsHash, name, URI, (void *) ext,
                        (xmlHashDeallocator) xsltFreeExtElement);

 done:
     xmlMutexUnlock(xsltExtMutex);
 
    return (0);
 }
