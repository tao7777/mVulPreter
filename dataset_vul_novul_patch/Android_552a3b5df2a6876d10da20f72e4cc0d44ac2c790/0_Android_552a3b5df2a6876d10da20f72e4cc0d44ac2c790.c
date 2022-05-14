sp<ABuffer> decodeBase64(const AString &s) {
 size_t n = s.size();
 if ((n % 4) != 0) {
 return NULL;
 }

 size_t padding = 0;
 if (n >= 1 && s.c_str()[n - 1] == '=') {
        padding = 1;

 if (n >= 2 && s.c_str()[n - 2] == '=') {
            padding = 2;

 if (n >= 3 && s.c_str()[n - 3] == '=') {
                padding = 3;
 }
 }
 }

 size_t outLen = (n / 4) * 3 - padding;

    sp<ABuffer> buffer = new ABuffer(outLen);

 uint8_t *out = buffer->data();
 if (out == NULL || buffer->size() < outLen) {
 return NULL;
 }
 size_t j = 0;
 uint32_t accum = 0;
 for (size_t i = 0; i < n; ++i) {
 char c = s.c_str()[i];
 unsigned value;
 if (c >= 'A' && c <= 'Z') {
            value = c - 'A';
 } else if (c >= 'a' && c <= 'z') {
            value = 26 + c - 'a';
 } else if (c >= '0' && c <= '9') {
            value = 52 + c - '0';
 } else if (c == '+') {
            value = 62;
 } else if (c == '/') {
            value = 63;
 } else if (c != '=') {
 return NULL;
 } else {
 if (i < n - padding) {
 return NULL;
 }

            value = 0;
 }


         accum = (accum << 6) | value;
 
         if (((i + 1) % 4) == 0) {
            if (j < outLen) { out[j++] = (accum >> 16); }
             if (j < outLen) { out[j++] = (accum >> 8) & 0xff; }
             if (j < outLen) { out[j++] = accum & 0xff; }
 
            accum = 0;
 }
 }

 return buffer;
}
