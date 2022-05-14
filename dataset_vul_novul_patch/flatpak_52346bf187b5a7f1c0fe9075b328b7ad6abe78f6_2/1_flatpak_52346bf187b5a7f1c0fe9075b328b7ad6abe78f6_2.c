side_in_cb (GSocket *socket, GIOCondition condition, gpointer user_data)
{
  ProxySide *side = user_data;
  FlatpakProxyClient *client = side->client;
  GError *error = NULL;
  Buffer *buffer;
  gboolean retval = G_SOURCE_CONTINUE;

  g_object_ref (client);

  while (!side->closed)
    {
      if (!side->got_first_byte)
        buffer = buffer_new (1, NULL);
      else if (!client->authenticated)
        buffer = buffer_new (64, NULL);
      else
        buffer = side->current_read_buffer;

      if (!buffer_read (side, buffer, socket))
        {
          if (buffer != side->current_read_buffer)
              buffer_unref (buffer);
          break;
        }

      if (!client->authenticated)
        {
          if (buffer->pos > 0)
            {
              gboolean found_auth_end = FALSE;
              gsize extra_data;

              buffer->size = buffer->pos;
              if (!side->got_first_byte)
                {
                  buffer->send_credentials = TRUE;
                  side->got_first_byte = TRUE;
                }
              /* Look for end of authentication mechanism */
              else if (side == &client->client_side)
                {
                  gssize auth_end = find_auth_end (client, buffer);

                  if (auth_end >= 0)
                    {
                      found_auth_end = TRUE;
                      buffer->size = auth_end;
                      extra_data = buffer->pos - buffer->size;

                      /* We may have gotten some extra data which is not part of
                         the auth handshake, keep it for the next iteration. */
                       if (extra_data > 0)
                         side->extra_input_data = g_bytes_new (buffer->data + buffer->size, extra_data);
                     }
                 }
 
               got_buffer_from_side (side, buffer);

              if (found_auth_end)
                client->authenticated = TRUE;
            }
          else
            {
              buffer_unref (buffer);
            }
        }
      else if (buffer->pos == buffer->size)
        {
          if (buffer == &side->header_buffer)
            {
              gssize required;
              required = g_dbus_message_bytes_needed (buffer->data, buffer->size, &error);
              if (required < 0)
                {
                  g_warning ("Invalid message header read");
                  side_closed (side);
                }
              else
                {
                  side->current_read_buffer = buffer_new (required, buffer);
                }
            }
          else
            {
              got_buffer_from_side (side, buffer);
              side->header_buffer.pos = 0;
              side->current_read_buffer = &side->header_buffer;
            }
        }
    }

  if (side->closed)
    {
      side->in_source = NULL;
      retval = G_SOURCE_REMOVE;
    }

  g_object_unref (client);

  return retval;
}
