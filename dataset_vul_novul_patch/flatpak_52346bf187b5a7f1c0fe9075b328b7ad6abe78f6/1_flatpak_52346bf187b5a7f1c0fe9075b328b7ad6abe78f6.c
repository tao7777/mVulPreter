 find_auth_end (FlatpakProxyClient *client, Buffer *buffer)
 {
  guchar *match;
  int i;
 
  /* First try to match any leftover at the start */
  if (client->auth_end_offset > 0)
     {
      gsize left = strlen (AUTH_END_STRING) - client->auth_end_offset;
      gsize to_match = MIN (left, buffer->pos);
      /* Matched at least up to to_match */
      if (memcmp (buffer->data, &AUTH_END_STRING[client->auth_end_offset], to_match) == 0)
         {
          client->auth_end_offset += to_match;
 
          /* Matched all */
          if (client->auth_end_offset == strlen (AUTH_END_STRING))
            return to_match;
 
          /* Matched to end of buffer */
          return -1;
        }
 
      /* Did not actually match at start */
      client->auth_end_offset = -1;
    }
 
  /* Look for whole match inside buffer */
  match = memmem (buffer, buffer->pos,
                  AUTH_END_STRING, strlen (AUTH_END_STRING));
  if (match != NULL)
    return match - buffer->data + strlen (AUTH_END_STRING);
 
  /* Record longest prefix match at the end */
  for (i = MIN (strlen (AUTH_END_STRING) - 1, buffer->pos); i > 0; i--)
    {
      if (memcmp (buffer->data + buffer->pos - i, AUTH_END_STRING, i) == 0)
        {
          client->auth_end_offset = i;
          break;
         }
     }
  return -1;
 }
