 static char* allocFromUTF32(const char32_t* in, size_t len)
{
 if (len == 0) {

         return getEmptyString();
     }
 
    const ssize_t resultStrLen = utf32_to_utf8_length(in, len) + 1;
    if (resultStrLen < 1) {
         return getEmptyString();
     }
 
    SharedBuffer* buf = SharedBuffer::alloc(resultStrLen);
     ALOG_ASSERT(buf, "Unable to allocate shared buffer");
     if (!buf) {
         return getEmptyString();
     }
 
    char* resultStr = (char*) buf->data();
    utf32_to_utf8(in, len, resultStr, resultStrLen);
 
    return resultStr;
 }
