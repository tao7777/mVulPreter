bool initiate_stratum(struct pool *pool)
{
	bool ret = false, recvd = false, noresume = false, sockd = false;
	char s[RBUFSIZE], *sret = NULL, *nonce1, *sessionid;
	json_t *val = NULL, *res_val, *err_val;
	json_error_t err;
	int n2size;

resend:
	if (!setup_stratum_socket(pool)) {
		sockd = false;
		goto out;
	}

	sockd = true;

	if (recvd) {
		/* Get rid of any crap lying around if we're resending */
		clear_sock(pool);
		sprintf(s, "{\"id\": %d, \"method\": \"mining.subscribe\", \"params\": []}", swork_id++);
	} else {
		if (pool->sessionid)
			sprintf(s, "{\"id\": %d, \"method\": \"mining.subscribe\", \"params\": [\""PACKAGE"/"VERSION"\", \"%s\"]}", swork_id++, pool->sessionid);
		else
			sprintf(s, "{\"id\": %d, \"method\": \"mining.subscribe\", \"params\": [\""PACKAGE"/"VERSION"\"]}", swork_id++);
	}

	if (__stratum_send(pool, s, strlen(s)) != SEND_OK) {
		applog(LOG_DEBUG, "Failed to send s in initiate_stratum");
		goto out;
	}

	if (!socket_full(pool, DEFAULT_SOCKWAIT)) {
		applog(LOG_DEBUG, "Timed out waiting for response in initiate_stratum");
		goto out;
	}

	sret = recv_line(pool);
	if (!sret)
		goto out;

	recvd = true;

	val = JSON_LOADS(sret, &err);
	free(sret);
	if (!val) {
		applog(LOG_INFO, "JSON decode failed(%d): %s", err.line, err.text);
		goto out;
	}

	res_val = json_object_get(val, "result");
	err_val = json_object_get(val, "error");

	if (!res_val || json_is_null(res_val) ||
	    (err_val && !json_is_null(err_val))) {
		char *ss;

		if (err_val)
			ss = json_dumps(err_val, JSON_INDENT(3));
		else
			ss = strdup("(unknown reason)");

		applog(LOG_INFO, "JSON-RPC decode failed: %s", ss);

		free(ss);

		goto out;
	}

	sessionid = get_sessionid(res_val);
 	if (!sessionid)
 		applog(LOG_DEBUG, "Failed to get sessionid in initiate_stratum");
 	nonce1 = json_array_string(res_val, 1);
	if (!valid_hex(nonce1)) {
		applog(LOG_INFO, "Failed to get valid nonce1 in initiate_stratum");
 		free(sessionid);
 		goto out;
 	}
 	n2size = json_integer_value(json_array_get(res_val, 2));
	if (n2size < 2 || n2size > 16) {
		applog(LOG_INFO, "Failed to get valid n2size in initiate_stratum");
 		free(sessionid);
 		free(nonce1);
 		goto out;
	}

	cg_wlock(&pool->data_lock);
	pool->sessionid = sessionid;
	pool->nonce1 = nonce1;
	pool->n1_len = strlen(nonce1) / 2;
	free(pool->nonce1bin);
	pool->nonce1bin = calloc(pool->n1_len, 1);
	if (unlikely(!pool->nonce1bin))
		quithere(1, "Failed to calloc pool->nonce1bin");
	hex2bin(pool->nonce1bin, pool->nonce1, pool->n1_len);
	pool->n2size = n2size;
	cg_wunlock(&pool->data_lock);

	if (sessionid)
		applog(LOG_DEBUG, "Pool %d stratum session id: %s", pool->pool_no, pool->sessionid);

	ret = true;
out:
	if (ret) {
		if (!pool->stratum_url)
			pool->stratum_url = pool->sockaddr_url;
		pool->stratum_active = true;
		pool->sdiff = 1;
		if (opt_protocol) {
			applog(LOG_DEBUG, "Pool %d confirmed mining.subscribe with extranonce1 %s extran2size %d",
			       pool->pool_no, pool->nonce1, pool->n2size);
		}
	} else {
		if (recvd && !noresume) {
			/* Reset the sessionid used for stratum resuming in case the pool
			* does not support it, or does not know how to respond to the
			* presence of the sessionid parameter. */
			cg_wlock(&pool->data_lock);
			free(pool->sessionid);
			free(pool->nonce1);
			pool->sessionid = pool->nonce1 = NULL;
			cg_wunlock(&pool->data_lock);

			applog(LOG_DEBUG, "Failed to resume stratum, trying afresh");
			noresume = true;
			json_decref(val);
			goto resend;
		}
		applog(LOG_DEBUG, "Initiate stratum failed");
		if (sockd)
			suspend_stratum(pool);
	}

	json_decref(val);
	return ret;
}
