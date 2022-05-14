char *url_decode_r(char *to, char *url, size_t size) {
    char *s = url,           // source
         *d = to,            // destination
         *e = &to[size - 1]; // destination end

     while(*s && d < e) {
         if(unlikely(*s == '%')) {
             if(likely(s[1] && s[2])) {
                char t = from_hex(s[1]) << 4 | from_hex(s[2]);
                // avoid HTTP header injection
                *d++ = (char)((isprint(t))? t : ' ');
                 s += 2;
             }
         }
        else if(unlikely(*s == '+'))
            *d++ = ' ';

        else
            *d++ = *s;

        s++;
    }

    *d = '\0';

    return to;
}
