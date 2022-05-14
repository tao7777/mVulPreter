 xmlBufCreateStatic(void *mem, size_t size) {
     xmlBufPtr ret;
 
    if (mem == NULL)
         return(NULL);
 
     ret = (xmlBufPtr) xmlMalloc(sizeof(xmlBuf));
    if (ret == NULL) {
	xmlBufMemoryError(NULL, "creating buffer");
        return(NULL);
    }
    if (size < INT_MAX) {
        ret->compat_use = size;
        ret->compat_size = size;
    } else {
        ret->compat_use = INT_MAX;
        ret->compat_size = INT_MAX;
    }
    ret->use = size;
    ret->size = size;
    ret->alloc = XML_BUFFER_ALLOC_IMMUTABLE;
    ret->content = (xmlChar *) mem;
    ret->error = 0;
    ret->buffer = NULL;
    return(ret);
}
