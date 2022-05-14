static int string_check(char *buf, const char *buf2)
static int _string_check(int linenumber, char *buf, const char *buf2)
 {
   if(strcmp(buf, buf2)) {
     /* they shouldn't differ */
    printf("sprintf line %d failed:\nwe      '%s'\nsystem: '%s'\n",
           linenumber, buf, buf2);
     return 1;
   }
   return 0;
 }
