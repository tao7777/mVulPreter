static int getnum (lua_State *L, const char **fmt, int df) {
static int getnum (const char **fmt, int df) {
   if (!isdigit(**fmt))  /* no number? */
     return df;  /* return default value */
   else {
     int a = 0;
     do {
       a = a*10 + *((*fmt)++) - '0';
     } while (isdigit(**fmt));
     return a;
  }
}
