char *curl_easy_unescape(CURL *handle, const char *string, int length,
                         int *olen)
 {
  int alloc = (length?length:(int)strlen(string))+1;
   char *ns = malloc(alloc);
   unsigned char in;
  int strindex=0;
   unsigned long hex;
   CURLcode res;
 
   if(!ns)
    return NULL;
 
   while(--alloc > 0) {
     in = *string;
    if(('%' == in) && ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      /* this is two hexadecimal digits following a '%' */
      char hexstr[3];
      char *ptr;
      hexstr[0] = string[1];
      hexstr[1] = string[2];
      hexstr[2] = 0;

      hex = strtoul(hexstr, &ptr, 16);
 
       in = curlx_ultouc(hex); /* this long is never bigger than 255 anyway */
 
      res = Curl_convert_from_network(handle, &in, 1);
       if(res) {
         /* Curl_convert_from_network calls failf if unsuccessful */
         free(ns);
        return NULL;
       }
 
       string+=2;
       alloc-=2;
     }
 
     ns[strindex++] = in;
     string++;
  }
  ns[strindex]=0; /* terminate it */

   if(olen)
     /* store output size */
     *olen = strindex;
  return ns;
 }
