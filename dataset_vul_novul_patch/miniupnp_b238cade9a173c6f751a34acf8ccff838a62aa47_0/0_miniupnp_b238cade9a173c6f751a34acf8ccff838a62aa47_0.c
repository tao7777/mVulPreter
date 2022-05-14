main(int argc, char * * argv)
{
	const char command0[] = { 0x00, 0x00 };
	char command1[] = "\x01\x00urn:schemas-upnp-org:device:InternetGatewayDevice";
	char command2[] = "\x02\x00uuid:fc4ec57e-b051-11db-88f8-0060085db3f6::upnp:rootdevice";
	const char command3[] = { 0x03, 0x00 };
	/* old versions of minissdpd would reject a command with
	 * a zero length string argument */
	char command3compat[] = "\x03\x00ssdp:all";
	char command4[] = "\x04\x00test:test:test";
 	const char bad_command[] = { 0xff, 0xff };
 	const char overflow[] = { 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
 	const char command5[] = { 0x05, 0x00 };
	const char bad_command4[] = { 0x04, 0x01, 0x60, 0x8f, 0xff, 0xff, 0xff, 0x7f};
 	int s;
 	int i;
 	void * tmp;
	unsigned char * resp = NULL;
	size_t respsize = 0;
	unsigned char buf[4096];
	ssize_t n;
	int total = 0;
	const char * sockpath = "/var/run/minissdpd.sock";

	for(i=0; i<argc-1; i++) {
		if(0==strcmp(argv[i], "-s"))
			sockpath = argv[++i];
	}
	command1[1] = sizeof(command1) - 3;
	command2[1] = sizeof(command2) - 3;
	command3compat[1] = sizeof(command3compat) - 3;
	command4[1] = sizeof(command4) - 3;
	s = connect_unix_socket(sockpath);

	n = SENDCOMMAND(command0, sizeof(command0));
	n = read(s, buf, sizeof(buf));
	printf("Response received %d bytes\n", (int)n);
	if(n > 0) {
		printversion(buf, n);
	} else {
		printf("Command 0 (get version) not supported\n");
		close(s);
		s = connect_unix_socket(sockpath);
	}

	n = SENDCOMMAND(command1, sizeof(command1) - 1);
	n = read(s, buf, sizeof(buf));
	printf("Response received %d bytes\n", (int)n);
	printresponse(buf, n);
	if(n == 0) {
		close(s);
		s = connect_unix_socket(sockpath);
	}

	n = SENDCOMMAND(command2, sizeof(command2) - 1);
	n = read(s, buf, sizeof(buf));
	printf("Response received %d bytes\n", (int)n);
	printresponse(buf, n);
	if(n == 0) {
		close(s);
		s = connect_unix_socket(sockpath);
	}

	buf[0] = 0; /* Slight hack for printing num devices when 0 */
	n = SENDCOMMAND(command3, sizeof(command3));
	n = read(s, buf, sizeof(buf));
	if(n == 0) {
		printf("command3 failed, testing compatible one\n");
		close(s);
		s = connect_unix_socket(sockpath);
		n = SENDCOMMAND(command3compat, sizeof(command3compat) - 1);
		n = read(s, buf, sizeof(buf));
	}
	printf("Response received %d bytes\n", (int)n);
	printf("Number of devices %d\n", (int)buf[0]);
	while(n > 0) {
		tmp = realloc(resp, respsize + n);
		if(tmp == NULL) {
			fprintf(stderr, "memory allocation error\n");
			break;
		}
		resp = tmp;
		respsize += n;
		if (n > 0) {
			memcpy(resp + total, buf, n);
			total += n;
		}
		if (n < (ssize_t)sizeof(buf)) {
			break;
		}

		n = read(s, buf, sizeof(buf));
		printf("response received %d bytes\n", (int)n);
	}
	if(resp != NULL) {
		printresponse(resp, total);
		free(resp);
		resp = NULL;
	}
	if(n == 0) {
		close(s);
		s = connect_unix_socket(sockpath);
	}

	n = SENDCOMMAND(command4, sizeof(command4));
	/* no response for request type 4 */

	n = SENDCOMMAND(bad_command, sizeof(bad_command));
	n = read(s, buf, sizeof(buf));
	printf("Response received %d bytes\n", (int)n);
	printresponse(buf, n);
	if(n == 0) {
		close(s);
		s = connect_unix_socket(sockpath);
	}

	n = SENDCOMMAND(overflow, sizeof(overflow));
	n = read(s, buf, sizeof(buf));
	printf("Response received %d bytes\n", (int)n);
	printresponse(buf, n);
	if(n == 0) {
		close(s);
		s = connect_unix_socket(sockpath);
	}

	n = SENDCOMMAND(command5, sizeof(command5));
 	n = read(s, buf, sizeof(buf));
 	printf("Response received %d bytes\n", (int)n);
 	printresponse(buf, n);
	if(n == 0) {
		close(s);
		s = connect_unix_socket(sockpath);
	}

	n = SENDCOMMAND(bad_command4, sizeof(bad_command4));
	n = read(s, buf, sizeof(buf));
	printf("Response received %d bytes\n", (int)n);
	printresponse(buf, n);
 
 	close(s);
 	return 0;
}
