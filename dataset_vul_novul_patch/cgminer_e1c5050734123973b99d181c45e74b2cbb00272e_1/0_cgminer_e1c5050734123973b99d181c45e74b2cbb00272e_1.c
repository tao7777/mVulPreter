static bool parse_notify(struct pool *pool, json_t *val)
{
	char *job_id, *prev_hash, *coinbase1, *coinbase2, *bbversion, *nbit,
	     *ntime, header[228];
	unsigned char *cb1 = NULL, *cb2 = NULL;
	size_t cb1_len, cb2_len, alloc_len;
	bool clean, ret = false;
	int merkles, i;
	json_t *arr;

	arr = json_array_get(val, 4);
	if (!arr || !json_is_array(arr))
		goto out;

	merkles = json_array_size(arr);

	job_id = json_array_string(val, 0);
	prev_hash = __json_array_string(val, 1);
	coinbase1 = json_array_string(val, 2);
	coinbase2 = json_array_string(val, 3);
	bbversion = __json_array_string(val, 5);
	nbit = __json_array_string(val, 6);
 	ntime = __json_array_string(val, 7);
 	clean = json_is_true(json_array_get(val, 8));
 
	if (!valid_hex(job_id) || !valid_hex(prev_hash) || !valid_hex(coinbase1) ||
	    !valid_hex(coinbase2) || !valid_hex(bbversion) || !valid_hex(nbit) ||
	    !valid_hex(ntime)) {
 		/* Annoying but we must not leak memory */
		free(job_id);
		free(coinbase1);
		free(coinbase2);
 		goto out;
 	}
 
	cg_wlock(&pool->data_lock);
	free(pool->swork.job_id);
	pool->swork.job_id = job_id;
	snprintf(pool->prev_hash, 65, "%s", prev_hash);
	cb1_len = strlen(coinbase1) / 2;
	cb2_len = strlen(coinbase2) / 2;
	snprintf(pool->bbversion, 9, "%s", bbversion);
	snprintf(pool->nbit, 9, "%s", nbit);
	snprintf(pool->ntime, 9, "%s", ntime);
	pool->swork.clean = clean;
	alloc_len = pool->coinbase_len = cb1_len + pool->n1_len + pool->n2size + cb2_len;
	pool->nonce2_offset = cb1_len + pool->n1_len;

	for (i = 0; i < pool->merkles; i++)
		free(pool->swork.merkle_bin[i]);
	if (merkles) {
		pool->swork.merkle_bin = realloc(pool->swork.merkle_bin,
						 sizeof(char *) * merkles + 1);
		for (i = 0; i < merkles; i++) {
			char *merkle = json_array_string(arr, i);

			pool->swork.merkle_bin[i] = malloc(32);
			if (unlikely(!pool->swork.merkle_bin[i]))
				quit(1, "Failed to malloc pool swork merkle_bin");
			if (opt_protocol)
				applog(LOG_DEBUG, "merkle %d: %s", i, merkle);
			ret = hex2bin(pool->swork.merkle_bin[i], merkle, 32);
			free(merkle);
			if (unlikely(!ret)) {
				applog(LOG_ERR, "Failed to convert merkle to merkle_bin in parse_notify");
				goto out_unlock;
			}
		}
	}
	pool->merkles = merkles;
	if (clean)
		pool->nonce2 = 0;
#if 0
	header_len = 		 strlen(pool->bbversion) +
				 strlen(pool->prev_hash);
	/* merkle_hash */	 32 +
				 strlen(pool->ntime) +
				 strlen(pool->nbit) +
	/* nonce */		 8 +
	/* workpadding */	 96;
#endif
	snprintf(header, 225,
		"%s%s%s%s%s%s%s",
		pool->bbversion,
		pool->prev_hash,
		blank_merkle,
		pool->ntime,
		pool->nbit,
		"00000000", /* nonce */
		workpadding);
	ret = hex2bin(pool->header_bin, header, 112);
	if (unlikely(!ret)) {
		applog(LOG_ERR, "Failed to convert header to header_bin in parse_notify");
		goto out_unlock;
	}

	cb1 = alloca(cb1_len);
	ret = hex2bin(cb1, coinbase1, cb1_len);
	if (unlikely(!ret)) {
		applog(LOG_ERR, "Failed to convert cb1 to cb1_bin in parse_notify");
		goto out_unlock;
	}
	cb2 = alloca(cb2_len);
	ret = hex2bin(cb2, coinbase2, cb2_len);
	if (unlikely(!ret)) {
		applog(LOG_ERR, "Failed to convert cb2 to cb2_bin in parse_notify");
		goto out_unlock;
	}
	free(pool->coinbase);
	align_len(&alloc_len);
	pool->coinbase = calloc(alloc_len, 1);
	if (unlikely(!pool->coinbase))
		quit(1, "Failed to calloc pool coinbase in parse_notify");
	memcpy(pool->coinbase, cb1, cb1_len);
	memcpy(pool->coinbase + cb1_len, pool->nonce1bin, pool->n1_len);
	memcpy(pool->coinbase + cb1_len + pool->n1_len + pool->n2size, cb2, cb2_len);
	if (opt_debug) {
		char *cb = bin2hex(pool->coinbase, pool->coinbase_len);

		applog(LOG_DEBUG, "Pool %d coinbase %s", pool->pool_no, cb);
		free(cb);
	}
out_unlock:
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

	/* A notify message is the closest stratum gets to a getwork */
	pool->getwork_requested++;
	total_getworks++;
	if (pool == current_pool())
		opt_work_update = true;
out:
	return ret;
}
