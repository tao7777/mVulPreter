void utf16_to_utf8(const char16_t* src, size_t src_len, char* dst)
 {
     if (src == NULL || src_len == 0 || dst == NULL) {
         return;
 }

 const char16_t* cur_utf16 = src;
 const char16_t* const end_utf16 = src + src_len;
 char *cur = dst;
 while (cur_utf16 < end_utf16) {
 char32_t utf32;
 if((*cur_utf16 & 0xFC00) == 0xD800 && (cur_utf16 + 1) < end_utf16
 && (*(cur_utf16 + 1) & 0xFC00) == 0xDC00) {
            utf32 = (*cur_utf16++ - 0xD800) << 10;
            utf32 |= *cur_utf16++ - 0xDC00;
            utf32 += 0x10000;
 } else {

             utf32 = (char32_t) *cur_utf16++;
         }
         const size_t len = utf32_codepoint_utf8_length(utf32);
         utf32_codepoint_to_utf8((uint8_t*)cur, utf32, len);
         cur += len;
     }
     *cur = '\0';
 }
