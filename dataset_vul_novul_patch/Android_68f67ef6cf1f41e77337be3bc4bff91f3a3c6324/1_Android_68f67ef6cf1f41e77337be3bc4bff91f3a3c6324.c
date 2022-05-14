bool ASessionDescription::getDimensions(
 size_t index, unsigned long PT,
 int32_t *width, int32_t *height) const {

     *width = 0;
     *height = 0;
 
    char key[20];
    sprintf(key, "a=framesize:%lu", PT);
     AString value;
     if (!findAttribute(index, key, &value)) {
         return false;
 }

 const char *s = value.c_str();
 char *end;
 *width = strtoul(s, &end, 10);
    CHECK_GT(end, s);
    CHECK_EQ(*end, '-');

    s = end + 1;
 *height = strtoul(s, &end, 10);
    CHECK_GT(end, s);
    CHECK_EQ(*end, '\0');

 return true;
}
