char *curl_easy_unescape(CURL *handle, const char *string, int length,
CURLcode Curl_urldecode(struct SessionHandle *data,
                        const char *string, size_t length,
                        char **ostring, size_t *olen,
                        bool reject_ctrl)
 {
  size_t alloc = (length?length:strlen(string))+1;
   char *ns = malloc(alloc);
   unsigned char in;
  size_t strindex=0;
   unsigned long hex;
   CURLcode res;
 
   if(!ns)
    return CURLE_OUT_OF_MEMORY;
 
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
 
      res = Curl_convert_from_network(data, &in, 1);
       if(res) {
         /* Curl_convert_from_network calls failf if unsuccessful */
         free(ns);
        return res;
       }
 
       string+=2;
       alloc-=2;
     }
    if(reject_ctrl && (in < 0x20)) {
      free(ns);
      return CURLE_URL_MALFORMAT;
    }
 
     ns[strindex++] = in;
     string++;
  }
  ns[strindex]=0; /* terminate it */

   if(olen)
     /* store output size */
     *olen = strindex;

  if(ostring)
    /* store output string */
    *ostring = ns;

  return CURLE_OK;
}

/*
 * Unescapes the given URL escaped string of given length. Returns a
 * pointer to a malloced string with length given in *olen.
 * If length == 0, the length is assumed to be strlen(string).
 * If olen == NULL, no output length is stored.
 */
char *curl_easy_unescape(CURL *handle, const char *string, int length,
                         int *olen)
{
  char *str = NULL;
  size_t inputlen = length;
  size_t outputlen;
  CURLcode res = Curl_urldecode(handle, string, inputlen, &str, &outputlen,
                                FALSE);
  if(res)
    return NULL;
  if(olen)
    *olen = curlx_uztosi(outputlen);
  return str;
 }
