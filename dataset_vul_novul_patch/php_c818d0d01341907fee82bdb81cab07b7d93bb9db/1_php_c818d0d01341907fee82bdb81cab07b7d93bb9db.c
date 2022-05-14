static int date_from_ISO8601 (const char *text, time_t * value) {
   struct tm tm;
 * Begin Time Functions *
 ***********************/
 
 static int date_from_ISO8601 (const char *text, time_t * value) {
    struct tm tm;
    int n;
    int i;
       char buf[18];
 
        if (strchr (text, '-')) {
                char *p = (char *) text, *p2 = buf;
   }
                        if (*p != '-') {
                                *p2 = *p;
                                p2++;
                        }
                        p++;
                }
   }
