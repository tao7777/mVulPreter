static void ssdp_recv(int sd)
{
 	ssize_t len;
 	struct sockaddr sa;
 	socklen_t salen;
	char buf[MAX_PKT_SIZE + 1];
 
 	memset(buf, 0, sizeof(buf));
	len = recvfrom(sd, buf, sizeof(buf) - 1, MSG_DONTWAIT, &sa, &salen);
 	if (len > 0) {
 		if (sa.sa_family != AF_INET)
 			return;
 
		if (strstr(buf, "M-SEARCH *")) {
			size_t i;
			char *ptr, *type;
			struct ifsock *ifs;
			struct sockaddr_in *sin = (struct sockaddr_in *)&sa;

			ifs = find_outbound(&sa);
			if (!ifs) {
				logit(LOG_DEBUG, "No matching socket for client %s", inet_ntoa(sin->sin_addr));
				return;
			}
			logit(LOG_DEBUG, "Matching socket for client %s", inet_ntoa(sin->sin_addr));

			type = strcasestr(buf, "\r\nST:");
			if (!type) {
				logit(LOG_DEBUG, "No Search Type (ST:) found in M-SEARCH *, assuming " SSDP_ST_ALL);
				type = SSDP_ST_ALL;
				send_message(ifs, type, &sa);
				return;
			}

			type = strchr(type, ':');
			if (!type)
				return;
			type++;
			while (isspace(*type))
				type++;

			ptr = strstr(type, "\r\n");
			if (!ptr)
				return;
			*ptr = 0;

			for (i = 0; supported_types[i]; i++) {
				if (!strcmp(supported_types[i], type)) {
					logit(LOG_DEBUG, "M-SEARCH * ST: %s from %s port %d", type,
					      inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
					send_message(ifs, type, &sa);
					return;
				}
			}

			logit(LOG_DEBUG, "M-SEARCH * for unsupported ST: %s from %s", type,
			      inet_ntoa(sin->sin_addr));
		}
	}
}
