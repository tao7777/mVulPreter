bool extract_sockaddr(char *url, char **sockaddr_url, char **sockaddr_port)
{
	char *url_begin, *url_end, *ipv6_begin, *ipv6_end, *port_start = NULL;
	char url_address[256], port[6];
	int url_len, port_len = 0;

	*sockaddr_url = url;
	url_begin = strstr(url, "//");
	if (!url_begin)
		url_begin = url;
	else
		url_begin += 2;

	/* Look for numeric ipv6 entries */
	ipv6_begin = strstr(url_begin, "[");
	ipv6_end = strstr(url_begin, "]");
	if (ipv6_begin && ipv6_end && ipv6_end > ipv6_begin)
		url_end = strstr(ipv6_end, ":");
	else
		url_end = strstr(url_begin, ":");
	if (url_end) {
		url_len = url_end - url_begin;
		port_len = strlen(url_begin) - url_len - 1;
		if (port_len < 1)
			return false;
		port_start = url_end + 1;
	} else
		url_len = strlen(url_begin);

 	if (url_len < 1)
 		return false;
 
	snprintf(url_address, 254, "%.*s", url_len, url_begin);
 
 	if (port_len) {
 		char *slash;

		snprintf(port, 6, "%.*s", port_len, port_start);
		slash = strchr(port, '/');
		if (slash)
			*slash = '\0';
	} else
		strcpy(port, "80");

	*sockaddr_port = strdup(port);
	*sockaddr_url = strdup(url_address);

	return true;
}
