 static byte parseHexByte(const char * &str) {
    if (str[0] == '\0') {
        ALOGE("Passed an empty string");
        return 0;
    }
     byte b = parseHexChar(str[0]);
    if (str[1] == '\0' || str[1] == ':') {
        str ++;
     } else {
         b = b << 4 | parseHexChar(str[1]);
        str += 2;
     }

    // Skip trailing delimiter if not at the end of the string.
    if (str[0] != '\0') {
        str++;
    }
    return b;
 }
