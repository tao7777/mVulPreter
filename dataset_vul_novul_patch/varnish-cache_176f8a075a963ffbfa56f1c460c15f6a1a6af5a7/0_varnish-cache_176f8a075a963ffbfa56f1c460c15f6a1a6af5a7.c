vbf_stp_error(struct worker *wrk, struct busyobj *bo)
{
	ssize_t l, ll, o;
	double now;
	uint8_t *ptr;
	struct vsb *synth_body;

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(bo, BUSYOBJ_MAGIC);
	CHECK_OBJ_NOTNULL(bo->fetch_objcore, OBJCORE_MAGIC);
	AN(bo->fetch_objcore->flags & OC_F_BUSY);
	assert(bo->director_state == DIR_S_NULL);

	wrk->stats->fetch_failed++;

	now = W_TIM_real(wrk);
	VSLb_ts_busyobj(bo, "Error", now);

	if (bo->fetch_objcore->stobj->stevedore != NULL)
		ObjFreeObj(bo->wrk, bo->fetch_objcore);


	HTTP_Setup(bo->beresp, bo->ws, bo->vsl, SLT_BerespMethod);
	http_PutResponse(bo->beresp, "HTTP/1.1", 503, "Backend fetch failed");
	http_TimeHeader(bo->beresp, "Date: ", now);
	http_SetHeader(bo->beresp, "Server: Varnish");

	bo->fetch_objcore->t_origin = now;
	if (!VTAILQ_EMPTY(&bo->fetch_objcore->objhead->waitinglist)) {
		/*
		 * If there is a waitinglist, it means that there is no
		 * grace-able object, so cache the error return for a
		 * short time, so the waiting list can drain, rather than
		 * each objcore on the waiting list sequentially attempt
		 * to fetch from the backend.
		 */
		bo->fetch_objcore->ttl = 1;
		bo->fetch_objcore->grace = 5;
		bo->fetch_objcore->keep = 5;
	} else {
		bo->fetch_objcore->ttl = 0;
		bo->fetch_objcore->grace = 0;
		bo->fetch_objcore->keep = 0;
	}

	synth_body = VSB_new_auto();
	AN(synth_body);

	VCL_backend_error_method(bo->vcl, wrk, NULL, bo, synth_body);

	AZ(VSB_finish(synth_body));

	if (wrk->handling == VCL_RET_ABANDON || wrk->handling == VCL_RET_FAIL) {
		VSB_destroy(&synth_body);
		return (F_STP_FAIL);
	}

	if (wrk->handling == VCL_RET_RETRY) {
		VSB_destroy(&synth_body);
		if (bo->retries++ < cache_param->max_retries)
			return (F_STP_RETRY);
		VSLb(bo->vsl, SLT_VCL_Error, "Too many retries, failing");
		return (F_STP_FAIL);
	}

	assert(wrk->handling == VCL_RET_DELIVER);

	bo->vfc->bo = bo;
	bo->vfc->wrk = bo->wrk;
	bo->vfc->oc = bo->fetch_objcore;
	bo->vfc->http = bo->beresp;
	bo->vfc->esi_req = bo->bereq;

	if (vbf_beresp2obj(bo)) {
		(void)VFP_Error(bo->vfc, "Could not get storage");
		VSB_destroy(&synth_body);
		return (F_STP_FAIL);
	}

	ll = VSB_len(synth_body);
	o = 0;
	while (ll > 0) {
 		l = ll;
 		if (VFP_GetStorage(bo->vfc, &l, &ptr) != VFP_OK)
 			break;
		if (l > ll)
			l = ll;
 		memcpy(ptr, VSB_data(synth_body) + o, l);
 		VFP_Extend(bo->vfc, l);
 		ll -= l;
		o += l;
	}
	AZ(ObjSetU64(wrk, bo->fetch_objcore, OA_LEN, o));
	VSB_destroy(&synth_body);
	HSH_Unbusy(wrk, bo->fetch_objcore);
	ObjSetState(wrk, bo->fetch_objcore, BOS_FINISHED);
	return (F_STP_DONE);
}
