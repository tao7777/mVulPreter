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
	cJSON_AddIntToObject(j, "omit", test->omit);
 	if (test->server_affinity != -1)
	    cJSON_AddIntToObject(j, "server_affinity", test->server_affinity);
 	if (test->duration)
	    cJSON_AddIntToObject(j, "time", test->duration);
 	if (test->settings->bytes)
	    cJSON_AddIntToObject(j, "num", test->settings->bytes);
 	if (test->settings->blocks)
	    cJSON_AddIntToObject(j, "blockcount", test->settings->blocks);
 	if (test->settings->mss)
	    cJSON_AddIntToObject(j, "MSS", test->settings->mss);
 	if (test->no_delay)
 	    cJSON_AddTrueToObject(j, "nodelay");
	cJSON_AddIntToObject(j, "parallel", test->num_streams);
 	if (test->reverse)
 	    cJSON_AddTrueToObject(j, "reverse");
 	if (test->settings->socket_bufsize)
	    cJSON_AddIntToObject(j, "window", test->settings->socket_bufsize);
 	if (test->settings->blksize)
	    cJSON_AddIntToObject(j, "len", test->settings->blksize);
 	if (test->settings->rate)
	    cJSON_AddIntToObject(j, "bandwidth", test->settings->rate);
 	if (test->settings->burst)
	    cJSON_AddIntToObject(j, "burst", test->settings->burst);
 	if (test->settings->tos)
	    cJSON_AddIntToObject(j, "TOS", test->settings->tos);
 	if (test->settings->flowlabel)
	    cJSON_AddIntToObject(j, "flowlabel", test->settings->flowlabel);
 	if (test->title)
 	    cJSON_AddStringToObject(j, "title", test->title);
 	if (test->congestion)
 	    cJSON_AddStringToObject(j, "congestion", test->congestion);
 	if (test->get_server_output)
 	    cJSON_AddIntToObject(j, "get_server_output", iperf_get_test_get_server_output(test));
 
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
