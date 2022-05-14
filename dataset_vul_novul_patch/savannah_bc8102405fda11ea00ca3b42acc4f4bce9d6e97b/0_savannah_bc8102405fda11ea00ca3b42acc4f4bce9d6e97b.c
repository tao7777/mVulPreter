_gnutls_server_name_recv_params (gnutls_session_t session,
				 const opaque * data, size_t _data_size)
{
  int i;
  const unsigned char *p;
  uint16_t len, type;
  ssize_t data_size = _data_size;
  int server_names = 0;

  if (session->security_parameters.entity == GNUTLS_SERVER)
    {
      DECR_LENGTH_RET (data_size, 2, 0);
      len = _gnutls_read_uint16 (data);

      if (len != data_size)
	{
	  /* This is unexpected packet length, but
	   * just ignore it, for now.
	   */
	  gnutls_assert ();
	  return 0;
	}

      p = data + 2;

      /* Count all server_names in the packet. */
      while (data_size > 0)
	{
	  DECR_LENGTH_RET (data_size, 1, 0);
	  p++;

	  DECR_LEN (data_size, 2);
          len = _gnutls_read_uint16 (p);
          p += 2;
 
         if (len > 0)
           {
             DECR_LENGTH_RET (data_size, len, 0);
             server_names++;
             p += len;
           }
         else
           _gnutls_handshake_log
             ("HSK[%x]: Received zero size server name (under attack?)\n",
              session);
 
       }

      /* we cannot accept more server names.
       */
      if (server_names > MAX_SERVER_NAME_EXTENSIONS)
       {
         _gnutls_handshake_log
           ("HSK[%x]: Too many server names received (under attack?)\n",
            session);
         server_names = MAX_SERVER_NAME_EXTENSIONS;
        }
 
       session->security_parameters.extensions.server_names_size =

       if (server_names == 0)
        return 0;               /* no names found */
 
 
       p = data + 2;
       for (i = 0; i < server_names; i++)
			  server_names[i].name, p, len);
		  session->security_parameters.extensions.
		    server_names[i].name_length = len;
		  session->security_parameters.extensions.
		    server_names[i].type = GNUTLS_NAME_DNS;
		  break;
		}
	    }
