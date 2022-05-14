 find_auth_end (FlatpakProxyClient *client, Buffer *buffer)
 {
  goffset offset = 0;
  gsize original_size = client->auth_buffer->len;

  /* Add the new data to the remaining data from last iteration */
  g_byte_array_append (client->auth_buffer, buffer->data, buffer->pos);
 
  while (TRUE)
     {
      guint8 *line_start = client->auth_buffer->data + offset;
      gsize remaining_data = client->auth_buffer->len - offset;
      guint8 *line_end;

      line_end = memmem (line_start, remaining_data,
                         AUTH_LINE_SENTINEL, strlen (AUTH_LINE_SENTINEL));
      if (line_end) /* Found end of line */
         {
          offset = (line_end + strlen (AUTH_LINE_SENTINEL) - line_start);
 
          if (!auth_line_is_valid (line_start, line_end))
            return FIND_AUTH_END_ABORT;
 
          *line_end = 0;
          if (auth_line_is_begin (line_start))
            return offset - original_size;
 
          /* continue with next line */
        }
      else
        {
          /* No end-of-line in this buffer */
          g_byte_array_remove_range (client->auth_buffer, 0, offset);
 
          /* Abort if more than 16k before newline, similar to what dbus-daemon does */
          if (client->auth_buffer->len >= 16*1024)
            return FIND_AUTH_END_ABORT;
 
          return FIND_AUTH_END_CONTINUE;
         }
     }
 }
