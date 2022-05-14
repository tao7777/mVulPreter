 static byte parseHexByte(const char * &str) {
     byte b = parseHexChar(str[0]);
    if (str[1] == ':' || str[1] == '\0') {
        str += 2;
        return b;
     } else {
         b = b << 4 | parseHexChar(str[1]);
        str += 3;
        return b;
     }
 }
