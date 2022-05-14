 static int is_integer(char *string)
 {
  if (isdigit(string[0]) || string[0] == '-' || string[0] == '+') {
    while (*++string && isdigit(*string))
       ;                                           /* deliberately empty */
     if (!*string)
       return 1;
  }
  return 0;
}
