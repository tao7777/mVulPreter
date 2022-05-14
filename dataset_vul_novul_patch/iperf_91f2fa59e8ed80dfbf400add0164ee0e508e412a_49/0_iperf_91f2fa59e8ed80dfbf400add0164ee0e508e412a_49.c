iperf_on_connect(struct iperf_test *test)
{
    time_t now_secs;
    const char* rfc1123_fmt = "%a, %d %b %Y %H:%M:%S GMT";
    char now_str[100];
    char ipr[INET6_ADDRSTRLEN];
    int port;
    struct sockaddr_storage sa;
    struct sockaddr_in *sa_inP;
    struct sockaddr_in6 *sa_in6P;
    socklen_t len;
    int opt;

    now_secs = time((time_t*) 0);
    (void) strftime(now_str, sizeof(now_str), rfc1123_fmt, gmtime(&now_secs));
    if (test->json_output)
	cJSON_AddItemToObject(test->json_start, "timestamp", iperf_json_printf("time: %s  timesecs: %d", now_str, (int64_t) now_secs));
    else if (test->verbose)
	iprintf(test, report_time, now_str);

    if (test->role == 'c') {
	if (test->json_output)
	    cJSON_AddItemToObject(test->json_start, "connecting_to", iperf_json_printf("host: %s  port: %d", test->server_hostname, (int64_t) test->server_port));
	else {
	    iprintf(test, report_connecting, test->server_hostname, test->server_port);
	    if (test->reverse)
		iprintf(test, report_reverse, test->server_hostname);
	}
    } else {
        len = sizeof(sa);
        getpeername(test->ctrl_sck, (struct sockaddr *) &sa, &len);
        if (getsockdomain(test->ctrl_sck) == AF_INET) {
	    sa_inP = (struct sockaddr_in *) &sa;
            inet_ntop(AF_INET, &sa_inP->sin_addr, ipr, sizeof(ipr));
	    port = ntohs(sa_inP->sin_port);
        } else {
	    sa_in6P = (struct sockaddr_in6 *) &sa;
            inet_ntop(AF_INET6, &sa_in6P->sin6_addr, ipr, sizeof(ipr));
	    port = ntohs(sa_in6P->sin6_port);
        }
	mapped_v4_to_regular_v4(ipr);
	if (test->json_output)
	    cJSON_AddItemToObject(test->json_start, "accepted_connection", iperf_json_printf("host: %s  port: %d", ipr, (int64_t) port));
	else
	    iprintf(test, report_accepted, ipr, port);
    }
    if (test->json_output) {
 	cJSON_AddStringToObject(test->json_start, "cookie", test->cookie);
         if (test->protocol->id == SOCK_STREAM) {
 	    if (test->settings->mss)
		cJSON_AddNumberToObject(test->json_start, "tcp_mss", test->settings->mss);
 	    else {
 		len = sizeof(opt);
 		getsockopt(test->ctrl_sck, IPPROTO_TCP, TCP_MAXSEG, &opt, &len);
		cJSON_AddNumberToObject(test->json_start, "tcp_mss_default", opt);
 	    }
 	}
     } else if (test->verbose) {
        iprintf(test, report_cookie, test->cookie);
        if (test->protocol->id == SOCK_STREAM) {
            if (test->settings->mss)
                iprintf(test, "      TCP MSS: %d\n", test->settings->mss);
            else {
                len = sizeof(opt);
                getsockopt(test->ctrl_sck, IPPROTO_TCP, TCP_MAXSEG, &opt, &len);
                iprintf(test, "      TCP MSS: %d (default)\n", opt);
            }
        }

    }
}
