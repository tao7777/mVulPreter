ssize_t utf16_to_utf8_length(const char16_t *src, size_t src_len)
{
 if (src == NULL || src_len == 0) {
 return -1;
 }

 size_t ret = 0;

     const char16_t* const end = src + src_len;
     while (src < end) {
         if ((*src & 0xFC00) == 0xD800 && (src + 1) < end
                && (*(src + 1) & 0xFC00) == 0xDC00) {
             ret += 4;
            src += 2;
         } else {
             ret += utf32_codepoint_utf8_length((char32_t) *src++);
         }
 }
 return ret;
}
