static void start_daemon()
{
	struct usb_sock_t *usb_sock;
	if (g_options.noprinter_mode == 0) {
		usb_sock = usb_open();
		if (usb_sock == NULL)
			goto cleanup_usb;
	} else
		usb_sock = NULL;
 
 	uint16_t desired_port = g_options.desired_port;
	struct tcp_sock_t *tcp_socket;
	while ((tcp_socket = tcp_open(desired_port)) == NULL &&
	       g_options.only_desired_port == 0) {
 		desired_port ++;
		if (desired_port == 1 || desired_port == 0)
 			desired_port = 49152;
 	}
	if (tcp_socket == NULL)
 		goto cleanup_tcp;
 
	uint16_t real_port = tcp_port_number_get(tcp_socket);
 	if (desired_port != 0 && g_options.only_desired_port == 1 &&
 	    desired_port != real_port) {
 		ERR("Received port number did not match requested port number."
		    " The requested port number may be too high.");
		goto cleanup_tcp;
	}
 	printf("%u|", real_port);
 	fflush(stdout);
 
 	uint16_t pid;
 	if (!g_options.nofork_mode && (pid = fork()) > 0) {
		printf("%u|", pid);
		exit(0);
	}

	if (usb_can_callback(usb_sock))
		usb_register_callback(usb_sock);

	for (;;) {
		struct service_thread_param *args = calloc(1, sizeof(*args));
		if (args == NULL) {
			ERR("Failed to alloc space for thread args");
			goto cleanup_thread;
 		}
 
 		args->usb_sock = usb_sock;
		args->tcp = tcp_conn_accept(tcp_socket);
 		if (args->tcp == NULL) {
 			ERR("Failed to open tcp connection");
 			goto cleanup_thread;
		}

		int status = pthread_create(&args->thread_handle, NULL,
		                            &service_connection, args);
		if (status) {
			ERR("Failed to spawn thread, error %d", status);
			goto cleanup_thread;
		}

		continue;

	cleanup_thread:
		if (args != NULL) {
			if (args->tcp != NULL)
				tcp_conn_close(args->tcp);
			free(args);
		}
		break;
	}

 cleanup_tcp:
 	if (tcp_socket!= NULL)
 		tcp_close(tcp_socket);
 cleanup_usb:
 	if (usb_sock != NULL)
 		usb_close(usb_sock);
	return;
}
