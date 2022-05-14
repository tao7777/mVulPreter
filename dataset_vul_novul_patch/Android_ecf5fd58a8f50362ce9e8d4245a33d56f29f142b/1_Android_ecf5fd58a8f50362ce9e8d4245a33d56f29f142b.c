static char* allocFromUTF16(const char16_t* in, size_t len)

 {
     if (len == 0) return getEmptyString();
 
    const ssize_t bytes = utf16_to_utf8_length(in, len);
    if (bytes < 0) {
         return getEmptyString();
     }
 
    SharedBuffer* buf = SharedBuffer::alloc(bytes+1);
     ALOG_ASSERT(buf, "Unable to allocate shared buffer");
     if (!buf) {
         return getEmptyString();
     }
 
    char* str = (char*)buf->data();
    utf16_to_utf8(in, len, str);
    return str;
 }
