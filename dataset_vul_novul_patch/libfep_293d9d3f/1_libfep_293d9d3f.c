_fep_open_control_socket (Fep *fep)
{
  struct sockaddr_un sun;
  char *path;
  int fd;
  ssize_t sun_len;

  fd = socket (AF_UNIX, SOCK_STREAM, 0);
  if (fd < 0)
    {
      perror ("socket");
      return -1;
    }

  path = create_socket_name ("fep-XXXXXX/control");
  if (strlen (path) + 1 >= sizeof(sun.sun_path))
    {
      fep_log (FEP_LOG_LEVEL_WARNING,
	       "unix domain socket path too long: %d + 1 >= %d",
	       strlen (path),
	       sizeof (sun.sun_path));
      free (path);
      return -1;
    }

   memset (&sun, 0, sizeof(sun));
   sun.sun_family = AF_UNIX;
 
#ifdef __linux__
  sun.sun_path[0] = '\0';
  memcpy (sun.sun_path + 1, path, strlen (path));
  sun_len = offsetof (struct sockaddr_un, sun_path) + strlen (path) + 1;
  remove_control_socket (path);
#else
   memcpy (sun.sun_path, path, strlen (path));
   sun_len = sizeof (struct sockaddr_un);
#endif
 
   if (bind (fd, (const struct sockaddr *) &sun, sun_len) < 0)
     {
      perror ("bind");
      free (path);
      close (fd);
      return -1;
    }

  if (listen (fd, 5) < 0)
    {
      perror ("listen");
      free (path);
      close (fd);
      return -1;
    }

  fep->server = fd;
  fep->control_socket_path = path;
  return 0;
}
