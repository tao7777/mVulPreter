static char* allocFromUTF16(const char16_t* in, size_t len)

 {
     if (len == 0) return getEmptyString();
 
     // Allow for closing '\0'
    const ssize_t resultStrLen = utf16_to_utf8_length(in, len) + 1;
    if (resultStrLen < 1) {
         return getEmptyString();
     }
 
    SharedBuffer* buf = SharedBuffer::alloc(resultStrLen);
     ALOG_ASSERT(buf, "Unable to allocate shared buffer");
     if (!buf) {
         return getEmptyString();
     }
 
    char* resultStr = (char*)buf->data();
    utf16_to_utf8(in, len, resultStr, resultStrLen);
    return resultStr;
 }
