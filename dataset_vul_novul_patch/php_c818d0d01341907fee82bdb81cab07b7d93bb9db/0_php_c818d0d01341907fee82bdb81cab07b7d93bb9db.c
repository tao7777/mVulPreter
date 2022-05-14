static int date_from_ISO8601 (const char *text, time_t * value) {
   struct tm tm;
 * Begin Time Functions *
 ***********************/
 
static time_t mkgmtime(struct tm *tm)
{
    static const int mdays[12] = {0,31,59,90,120,151,181,212,243,273,304,334};

    return ((((((tm->tm_year - 70) * 365) + mdays[tm->tm_mon] + tm->tm_mday-1 +
                  (tm->tm_year-68-1+(tm->tm_mon>=2))/4) * 24) + tm->tm_hour) * 60 +
        tm->tm_min) * 60 + tm->tm_sec;
}

 static int date_from_ISO8601 (const char *text, time_t * value) {
    struct tm tm;
    int n;
    int i;
   char buf[30];
       
 
        if (strchr (text, '-')) {
                char *p = (char *) text, *p2 = buf;
   }
                        if (*p != '-') {
                                *p2 = *p;
                                p2++;
                               if (p2-buf >= sizeof(buf)) {
                                       return -1;
                               }
                        }
                        p++;
                }
   }
