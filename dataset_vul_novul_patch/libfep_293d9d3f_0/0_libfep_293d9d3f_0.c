fep_client_open (const char *address)
{
  FepClient *client;
  struct sockaddr_un sun;
  ssize_t sun_len;
  int retval;

  if (!address)
    address = getenv ("LIBFEP_CONTROL_SOCK");
  if (!address)
    return NULL;

  if (strlen (address) + 1 >= sizeof(sun.sun_path))
    {
      fep_log (FEP_LOG_LEVEL_WARNING,
	       "unix domain socket path too long: %d + 1 >= %d",
	       strlen (address),
	       sizeof (sun.sun_path));
      free (address);
      return NULL;
    }

  client = xzalloc (sizeof(FepClient));
  client->filter_running = false;
  client->messages = NULL;

   memset (&sun, 0, sizeof(struct sockaddr_un));
   sun.sun_family = AF_UNIX;
 
   memcpy (sun.sun_path, address, strlen (address));
   sun_len = sizeof (struct sockaddr_un);
 
   client->control = socket (AF_UNIX, SOCK_STREAM, 0);
   if (client->control < 0)
    {
      free (client);
      return NULL;
    }

  retval = connect (client->control,
		    (const struct sockaddr *) &sun,
		    sun_len);
  if (retval < 0)
    {
      close (client->control);
      free (client);
      return NULL;
    }

  return client;
}
