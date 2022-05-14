void utf32_to_utf8(const char32_t* src, size_t src_len, char* dst)
 {
     if (src == NULL || src_len == 0 || dst == NULL) {
         return;
 }

 const char32_t *cur_utf32 = src;
 const char32_t *end_utf32 = src + src_len;

     char *cur = dst;
     while (cur_utf32 < end_utf32) {
         size_t len = utf32_codepoint_utf8_length(*cur_utf32);
         utf32_codepoint_to_utf8((uint8_t *)cur, *cur_utf32++, len);
         cur += len;
     }
     *cur = '\0';
 }
