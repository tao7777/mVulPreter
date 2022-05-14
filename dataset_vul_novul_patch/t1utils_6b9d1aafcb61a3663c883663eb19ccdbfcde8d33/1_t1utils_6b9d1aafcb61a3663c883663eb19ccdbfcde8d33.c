 static int check_line_charstring(void)
 {
   char *p = line;
  while (isspace(*p))
     p++;
   return (*p == '/' || (p[0] == 'd' && p[1] == 'u' && p[2] == 'p'));
 }
