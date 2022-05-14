gboolean lxterminal_socket_initialize(LXTermWindow * lxtermwin, gint argc, gchar * * argv)
{
    /* Normally, LXTerminal uses one process to control all of its windows.
     * The first process to start will create a Unix domain socket in /tmp.
     * It will then bind and listen on this socket.
     * The subsequent processes will connect to the controller that owns the Unix domain socket.
     * They will pass their command line over the socket and exit.
     *
     * If for any reason both the connect and bind fail, we will fall back to having that
     * process be standalone; it will not be either the controller or a user of the controller.
     * This behavior was introduced in response to a problem report (2973537).
     *
      * This function returns TRUE if this process should keep running and FALSE if it should exit. */
 
     /* Formulate the path for the Unix domain socket. */
    gchar * socket_path = g_strdup_printf("%s/.lxterminal-socket-%s", g_get_user_runtime_dir(), gdk_display_get_name(gdk_display_get_default()));
    printf("%s\n", socket_path);
 
     /* Create socket. */
     int fd = socket(PF_UNIX, SOCK_STREAM, 0);
    {
        g_warning("Socket create failed: %s\n", g_strerror(errno));
        g_free(socket_path);
        return TRUE;
    }

    /* Initialize socket address for Unix domain socket. */
    struct sockaddr_un sock_addr;
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    snprintf(sock_addr.sun_path, sizeof(sock_addr.sun_path), "%s", socket_path);

    /* Try to connect to an existing LXTerminal process. */
    if (connect(fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0)
    {
        /* Connect failed.  We are the controller, unless something fails. */
        unlink(socket_path);
        g_free(socket_path);

        /* Bind to socket. */
        if (bind(fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0)
        {
            g_warning("Bind on socket failed: %s\n", g_strerror(errno));
            close(fd);
            return TRUE;
        }

        /* Listen on socket. */
        if (listen(fd, 5) < 0)
        {
            g_warning("Listen on socket failed: %s\n", g_strerror(errno));
            close(fd);
            return TRUE;
        }

        /* Create a glib I/O channel. */
        GIOChannel * gio = g_io_channel_unix_new(fd);
        if (gio == NULL)
        {
            g_warning("Cannot create GIOChannel\n");
            close(fd);
            return TRUE;
        }

        /* Set up GIOChannel. */
        g_io_channel_set_encoding(gio, NULL, NULL);
        g_io_channel_set_buffered(gio, FALSE);
        g_io_channel_set_close_on_unref(gio, TRUE);

        /* Add I/O channel to the main event loop. */
        if ( ! g_io_add_watch(gio, G_IO_IN | G_IO_HUP, (GIOFunc) lxterminal_socket_accept_client, lxtermwin))
        {
            g_warning("Cannot add watch on GIOChannel\n");
            close(fd);
            g_io_channel_unref(gio);
            return TRUE;
        }

        /* Channel will automatically shut down when the watch returns FALSE. */
        g_io_channel_set_close_on_unref(gio, TRUE);
        g_io_channel_unref(gio);
        return TRUE;
    }
    else
    {
        g_free(socket_path);

        /* Create a glib I/O channel. */
        GIOChannel * gio = g_io_channel_unix_new(fd);
        g_io_channel_set_encoding(gio, NULL, NULL);

        /* Push current dir in case it is needed later */
	gchar * cur_dir = g_get_current_dir();
        g_io_channel_write_chars(gio, cur_dir, -1, NULL, NULL);
	/* Use "" as a pointer to '\0' since g_io_channel_write_chars() won't accept NULL */
	g_io_channel_write_chars(gio, "", 1, NULL, NULL);
	g_free(cur_dir);

        /* push all of argv. */
	gint i;
	for (i = 0; i < argc; i ++)
	{
            g_io_channel_write_chars(gio, argv[i], -1, NULL, NULL);
	    g_io_channel_write_chars(gio, "", 1, NULL, NULL);
	}

        g_io_channel_flush(gio, NULL);
        g_io_channel_unref(gio);
        return FALSE;
    }
}
