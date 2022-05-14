CURLcode Curl_auth_create_plain_message(struct Curl_easy *data,
                                        const char *userp,
                                        const char *passwdp,
                                        char **outptr, size_t *outlen)
{
  CURLcode result;
  char *plainauth;
  size_t ulen;
  size_t plen;
  size_t plainlen;

  *outlen = 0;
  *outptr = NULL;
  ulen = strlen(userp);
   plen = strlen(passwdp);
 
   /* Compute binary message length. Check for overflows. */
  if((ulen > SIZE_T_MAX/4) || (plen > (SIZE_T_MAX/2 - 2)))
     return CURLE_OUT_OF_MEMORY;
   plainlen = 2 * ulen + plen + 2;
 
  plainauth = malloc(plainlen);
  if(!plainauth)
    return CURLE_OUT_OF_MEMORY;

  /* Calculate the reply */
  memcpy(plainauth, userp, ulen);
  plainauth[ulen] = '\0';
  memcpy(plainauth + ulen + 1, userp, ulen);
  plainauth[2 * ulen + 1] = '\0';
  memcpy(plainauth + 2 * ulen + 2, passwdp, plen);

  /* Base64 encode the reply */
  result = Curl_base64_encode(data, plainauth, plainlen, outptr, outlen);
  free(plainauth);

  return result;
}
