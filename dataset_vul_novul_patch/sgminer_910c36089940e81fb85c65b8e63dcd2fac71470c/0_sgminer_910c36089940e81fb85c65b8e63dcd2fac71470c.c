static bool parse_notify(struct pool *pool, json_t *val)
{
	char *job_id, *prev_hash, *coinbase1, *coinbase2, *bbversion, *nbit,
	     *ntime, *header;
	size_t cb1_len, cb2_len, alloc_len;
	unsigned char *cb1, *cb2;
	bool clean, ret = false;
	int merkles, i;
	json_t *arr;

	arr = json_array_get(val, 4);
	if (!arr || !json_is_array(arr))
		goto out;

	merkles = json_array_size(arr);

	job_id = json_array_string(val, 0);
	prev_hash = json_array_string(val, 1);
	coinbase1 = json_array_string(val, 2);
	coinbase2 = json_array_string(val, 3);
	bbversion = json_array_string(val, 5);
	nbit = json_array_string(val, 6);
	ntime = json_array_string(val, 7);
	clean = json_is_true(json_array_get(val, 8));

	if (!job_id || !prev_hash || !coinbase1 || !coinbase2 || !bbversion || !nbit || !ntime) {
		/* Annoying but we must not leak memory */
		if (job_id)
			free(job_id);
		if (prev_hash)
			free(prev_hash);
		if (coinbase1)
			free(coinbase1);
		if (coinbase2)
			free(coinbase2);
		if (bbversion)
			free(bbversion);
		if (nbit)
			free(nbit);
		if (ntime)
			free(ntime);
		goto out;
	}

	cg_wlock(&pool->data_lock);
	free(pool->swork.job_id);
	free(pool->swork.prev_hash);
	free(pool->swork.bbversion);
	free(pool->swork.nbit);
	free(pool->swork.ntime);
	pool->swork.job_id = job_id;
	pool->swork.prev_hash = prev_hash;
	cb1_len = strlen(coinbase1) / 2;
	cb2_len = strlen(coinbase2) / 2;
	pool->swork.bbversion = bbversion;
	pool->swork.nbit = nbit;
	pool->swork.ntime = ntime;
	pool->swork.clean = clean;
	alloc_len = pool->swork.cb_len = cb1_len + pool->n1_len + pool->n2size + cb2_len;
	pool->nonce2_offset = cb1_len + pool->n1_len;

	for (i = 0; i < pool->swork.merkles; i++)
		free(pool->swork.merkle_bin[i]);
	if (merkles) {
		pool->swork.merkle_bin = (unsigned char **)realloc(pool->swork.merkle_bin,
						 sizeof(char *) * merkles + 1);
		for (i = 0; i < merkles; i++) {
			char *merkle = json_array_string(arr, i);

			pool->swork.merkle_bin[i] = (unsigned char *)malloc(32);
			if (unlikely(!pool->swork.merkle_bin[i]))
				quit(1, "Failed to malloc pool swork merkle_bin");
			hex2bin(pool->swork.merkle_bin[i], merkle, 32);
			free(merkle);
		}
	}
	pool->swork.merkles = merkles;
	if (clean)
		pool->nonce2 = 0;
	pool->merkle_offset = strlen(pool->swork.bbversion) +
			      strlen(pool->swork.prev_hash);
	pool->swork.header_len = pool->merkle_offset +
	/* merkle_hash */	 32 +
				 strlen(pool->swork.ntime) +
				 strlen(pool->swork.nbit) +
	/* nonce */		 8 +
	/* workpadding */	 96;
	pool->merkle_offset /= 2;
	pool->swork.header_len = pool->swork.header_len * 2 + 1;
	align_len(&pool->swork.header_len);
	header = (char *)alloca(pool->swork.header_len);
	snprintf(header, pool->swork.header_len,
		"%s%s%s%s%s%s%s",
		pool->swork.bbversion,
		pool->swork.prev_hash,
		blank_merkel,
		pool->swork.ntime,
 		pool->swork.nbit,
 		"00000000", /* nonce */
 		workpadding);
	if (unlikely(!hex2bin(pool->header_bin, header, 128))) {
		applog(LOG_WARNING, "%s: Failed to convert header to header_bin, got %s", __func__, header);
		pool_failed(pool);
		// TODO: memory leaks? goto out, clean up there?
		return false;
	}
 
 	cb1 = (unsigned char *)calloc(cb1_len, 1);
 	if (unlikely(!cb1))
 		quithere(1, "Failed to calloc cb1 in parse_notify");
 	hex2bin(cb1, coinbase1, cb1_len);

 	cb2 = (unsigned char *)calloc(cb2_len, 1);
 	if (unlikely(!cb2))
 		quithere(1, "Failed to calloc cb2 in parse_notify");
 	hex2bin(cb2, coinbase2, cb2_len);

 	free(pool->coinbase);
 	align_len(&alloc_len);
 	pool->coinbase = (unsigned char *)calloc(alloc_len, 1);
 	if (unlikely(!pool->coinbase))
 		quit(1, "Failed to calloc pool coinbase in parse_notify");
 	memcpy(pool->coinbase, cb1, cb1_len);
 	memcpy(pool->coinbase + cb1_len, pool->nonce1bin, pool->n1_len);
	// NOTE: gap for nonce2, filled at work generation time
 	memcpy(pool->coinbase + cb1_len + pool->n1_len + pool->n2size, cb2, cb2_len);
 	cg_wunlock(&pool->data_lock);
 
	if (opt_protocol) {
		applog(LOG_DEBUG, "job_id: %s", job_id);
		applog(LOG_DEBUG, "prev_hash: %s", prev_hash);
		applog(LOG_DEBUG, "coinbase1: %s", coinbase1);
		applog(LOG_DEBUG, "coinbase2: %s", coinbase2);
		applog(LOG_DEBUG, "bbversion: %s", bbversion);
		applog(LOG_DEBUG, "nbit: %s", nbit);
		applog(LOG_DEBUG, "ntime: %s", ntime);
		applog(LOG_DEBUG, "clean: %s", clean ? "yes" : "no");
	}
	free(coinbase1);
	free(coinbase2);
	free(cb1);
	free(cb2);

	/* A notify message is the closest stratum gets to a getwork */
	pool->getwork_requested++;
	total_getworks++;
	ret = true;
	if (pool == current_pool())
		opt_work_update = true;
out:
	return ret;
}
