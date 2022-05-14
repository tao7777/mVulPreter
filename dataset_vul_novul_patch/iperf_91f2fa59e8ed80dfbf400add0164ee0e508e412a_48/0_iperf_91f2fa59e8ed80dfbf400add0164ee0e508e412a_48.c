get_results(struct iperf_test *test)
{
    int r = 0;
    cJSON *j;
    cJSON *j_cpu_util_total;
    cJSON *j_cpu_util_user;
    cJSON *j_cpu_util_system;
    cJSON *j_sender_has_retransmits;
    int result_has_retransmits;
    cJSON *j_streams;
    int n, i;
    cJSON *j_stream;
    cJSON *j_id;
    cJSON *j_bytes;
    cJSON *j_retransmits;
    cJSON *j_jitter;
    cJSON *j_errors;
    cJSON *j_packets;
    cJSON *j_server_output;
    int sid, cerror, pcount;
    double jitter;
    iperf_size_t bytes_transferred;
    int retransmits;
    struct iperf_stream *sp;

    j = JSON_read(test->ctrl_sck);
    if (j == NULL) {
	i_errno = IERECVRESULTS;
        r = -1;
    } else {
	j_cpu_util_total = cJSON_GetObjectItem(j, "cpu_util_total");
	j_cpu_util_user = cJSON_GetObjectItem(j, "cpu_util_user");
	j_cpu_util_system = cJSON_GetObjectItem(j, "cpu_util_system");
	j_sender_has_retransmits = cJSON_GetObjectItem(j, "sender_has_retransmits");
	if (j_cpu_util_total == NULL || j_cpu_util_user == NULL || j_cpu_util_system == NULL || j_sender_has_retransmits == NULL) {
	    i_errno = IERECVRESULTS;
	    r = -1;
	} else {
	    if (test->debug) {
 		printf("get_results\n%s\n", cJSON_Print(j));
 	    }
 
	    test->remote_cpu_util[0] = j_cpu_util_total->valuedouble;
	    test->remote_cpu_util[1] = j_cpu_util_user->valuedouble;
	    test->remote_cpu_util[2] = j_cpu_util_system->valuedouble;
 	    result_has_retransmits = j_sender_has_retransmits->valueint;
 	    if (! test->sender)
 		test->sender_has_retransmits = result_has_retransmits;
	    j_streams = cJSON_GetObjectItem(j, "streams");
	    if (j_streams == NULL) {
		i_errno = IERECVRESULTS;
		r = -1;
	    } else {
	        n = cJSON_GetArraySize(j_streams);
		for (i=0; i<n; ++i) {
		    j_stream = cJSON_GetArrayItem(j_streams, i);
		    if (j_stream == NULL) {
			i_errno = IERECVRESULTS;
			r = -1;
		    } else {
			j_id = cJSON_GetObjectItem(j_stream, "id");
			j_bytes = cJSON_GetObjectItem(j_stream, "bytes");
			j_retransmits = cJSON_GetObjectItem(j_stream, "retransmits");
			j_jitter = cJSON_GetObjectItem(j_stream, "jitter");
			j_errors = cJSON_GetObjectItem(j_stream, "errors");
			j_packets = cJSON_GetObjectItem(j_stream, "packets");
			if (j_id == NULL || j_bytes == NULL || j_retransmits == NULL || j_jitter == NULL || j_errors == NULL || j_packets == NULL) {
			    i_errno = IERECVRESULTS;
			    r = -1;
			} else {
 			    sid = j_id->valueint;
 			    bytes_transferred = j_bytes->valueint;
 			    retransmits = j_retransmits->valueint;
			    jitter = j_jitter->valuedouble;
 			    cerror = j_errors->valueint;
 			    pcount = j_packets->valueint;
 			    SLIST_FOREACH(sp, &test->streams, streams)
				if (sp->id == sid) break;
			    if (sp == NULL) {
				i_errno = IESTREAMID;
				r = -1;
			    } else {
				if (test->sender) {
				    sp->jitter = jitter;
				    sp->cnt_error = cerror;
				    sp->packet_count = pcount;
				    sp->result->bytes_received = bytes_transferred;
				} else {
				    sp->result->bytes_sent = bytes_transferred;
				    sp->result->stream_retrans = retransmits;
				}
			    }
			}
		    }
		}
		/*
		 * If we're the client and we're supposed to get remote results,
		 * look them up and process accordingly.
		 */
		if (test->role == 'c' && iperf_get_test_get_server_output(test)) {
		    /* Look for JSON.  If we find it, grab the object so it doesn't get deleted. */
		    j_server_output = cJSON_DetachItemFromObject(j, "server_output_json");
		    if (j_server_output != NULL) {
			test->json_server_output = j_server_output;
		    }
		    else {
			/* No JSON, look for textual output.  Make a copy of the text for later. */
			j_server_output = cJSON_GetObjectItem(j, "server_output_text");
			if (j_server_output != NULL) {
			    test->server_output_text = strdup(j_server_output->valuestring);
			}
		    }
		}
	    }
	}
	cJSON_Delete(j);
    }
    return r;
}
