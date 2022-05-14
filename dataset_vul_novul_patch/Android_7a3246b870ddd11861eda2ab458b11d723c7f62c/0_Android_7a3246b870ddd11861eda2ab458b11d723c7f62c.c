static size_t StringSize(const uint8_t *start, uint8_t encoding) {
//// return includes terminator;  if unterminated, returns > limit
static size_t StringSize(const uint8_t *start, size_t limit, uint8_t encoding) {

     if (encoding == 0x00 || encoding == 0x03) {
        return strnlen((const char *)start, limit) + 1;
     }
 
     size_t n = 0;
    while ((n+1 < limit) && (start[n] != '\0' || start[n + 1] != '\0')) {
         n += 2;
     }
    n += 2;
    return n;
 }
