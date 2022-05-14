send_parameters(struct iperf_test *test)
{
    int r = 0;
    cJSON *j;

    j = cJSON_CreateObject();
    if (j == NULL) {
	i_errno = IESENDPARAMS;
	r = -1;
    } else {
	if (test->protocol->id == Ptcp)
 	    cJSON_AddTrueToObject(j, "tcp");
 	else if (test->protocol->id == Pudp)
 	    cJSON_AddTrueToObject(j, "udp");
	cJSON_AddNumberToObject(j, "omit", test->omit);
 	if (test->server_affinity != -1)
	    cJSON_AddNumberToObject(j, "server_affinity", test->server_affinity);
 	if (test->duration)
	    cJSON_AddNumberToObject(j, "time", test->duration);
 	if (test->settings->bytes)
	    cJSON_AddNumberToObject(j, "num", test->settings->bytes);
 	if (test->settings->blocks)
	    cJSON_AddNumberToObject(j, "blockcount", test->settings->blocks);
 	if (test->settings->mss)
	    cJSON_AddNumberToObject(j, "MSS", test->settings->mss);
 	if (test->no_delay)
 	    cJSON_AddTrueToObject(j, "nodelay");
	cJSON_AddNumberToObject(j, "parallel", test->num_streams);
 	if (test->reverse)
 	    cJSON_AddTrueToObject(j, "reverse");
 	if (test->settings->socket_bufsize)
	    cJSON_AddNumberToObject(j, "window", test->settings->socket_bufsize);
 	if (test->settings->blksize)
	    cJSON_AddNumberToObject(j, "len", test->settings->blksize);
 	if (test->settings->rate)
	    cJSON_AddNumberToObject(j, "bandwidth", test->settings->rate);
 	if (test->settings->burst)
	    cJSON_AddNumberToObject(j, "burst", test->settings->burst);
 	if (test->settings->tos)
	    cJSON_AddNumberToObject(j, "TOS", test->settings->tos);
 	if (test->settings->flowlabel)
	    cJSON_AddNumberToObject(j, "flowlabel", test->settings->flowlabel);
 	if (test->title)
 	    cJSON_AddStringToObject(j, "title", test->title);
 	if (test->congestion)
 	    cJSON_AddStringToObject(j, "congestion", test->congestion);
 	if (test->get_server_output)
<<<<<<< HEAD
 	    cJSON_AddIntToObject(j, "get_server_output", iperf_get_test_get_server_output(test));
=======
	    cJSON_AddNumberToObject(j, "get_server_output", iperf_get_test_get_server_output(test));
	if (test->udp_counters_64bit)
	    cJSON_AddNumberToObject(j, "udp_counters_64bit", iperf_get_test_udp_counters_64bit(test));
	if (test->no_fq_socket_pacing)
	    cJSON_AddNumberToObject(j, "no_fq_socket_pacing", iperf_get_no_fq_socket_pacing(test));

	cJSON_AddStringToObject(j, "client_version", IPERF_VERSION);
>>>>>>> ed94082... Fix a buffer overflow / heap corruption issue that could occur if a
 
 	if (test->debug) {
 	    printf("send_parameters:\n%s\n", cJSON_Print(j));
	}

	if (JSON_write(test->ctrl_sck, j) < 0) {
	    i_errno = IESENDPARAMS;
	    r = -1;
	}
	cJSON_Delete(j);
    }
    return r;
}
