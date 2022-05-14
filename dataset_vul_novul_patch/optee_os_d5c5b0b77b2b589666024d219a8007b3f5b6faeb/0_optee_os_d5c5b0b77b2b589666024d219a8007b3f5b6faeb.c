static TEE_Result tee_svc_copy_param(struct tee_ta_session *sess,
				     struct tee_ta_session *called_sess,
				     struct utee_params *callee_params,
				     struct tee_ta_param *param,
				     void *tmp_buf_va[TEE_NUM_PARAMS],
				     struct mobj **mobj_tmp)
{
	size_t n;
	TEE_Result res;
	size_t req_mem = 0;
	size_t s;
	uint8_t *dst = 0;
	bool ta_private_memref[TEE_NUM_PARAMS];
	struct user_ta_ctx *utc = to_user_ta_ctx(sess->ctx);
	void *va;
	size_t dst_offs;

	/* fill 'param' input struct with caller params description buffer */
	if (!callee_params) {
		memset(param, 0, sizeof(*param));
	} else {
		res = tee_mmu_check_access_rights(utc,
			TEE_MEMORY_ACCESS_READ | TEE_MEMORY_ACCESS_ANY_OWNER,
 			(uaddr_t)callee_params, sizeof(struct utee_params));
 		if (res != TEE_SUCCESS)
 			return res;
		res = utee_param_to_param(utc, param, callee_params);
		if (res != TEE_SUCCESS)
			return res;
 	}
 
 	if (called_sess && is_pseudo_ta_ctx(called_sess->ctx)) {
		/* pseudo TA borrows the mapping of the calling TA */
		return TEE_SUCCESS;
	}

	/* All mobj in param are of type MOJB_TYPE_VIRT */

	for (n = 0; n < TEE_NUM_PARAMS; n++) {

		ta_private_memref[n] = false;

		switch (TEE_PARAM_TYPE_GET(param->types, n)) {
		case TEE_PARAM_TYPE_MEMREF_INPUT:
		case TEE_PARAM_TYPE_MEMREF_OUTPUT:
		case TEE_PARAM_TYPE_MEMREF_INOUT:
			va = (void *)param->u[n].mem.offs;
			s = param->u[n].mem.size;
			if (!va) {
				if (s)
					return TEE_ERROR_BAD_PARAMETERS;
				break;
			}
			/* uTA cannot expose its private memory */
			if (tee_mmu_is_vbuf_inside_ta_private(utc, va, s)) {

				s = ROUNDUP(s, sizeof(uint32_t));
				if (ADD_OVERFLOW(req_mem, s, &req_mem))
					return TEE_ERROR_BAD_PARAMETERS;
				ta_private_memref[n] = true;
				break;
			}

			res = tee_mmu_vbuf_to_mobj_offs(utc, va, s,
							&param->u[n].mem.mobj,
							&param->u[n].mem.offs);
			if (res != TEE_SUCCESS)
				return res;
			break;
		default:
			break;
		}
	}

	if (req_mem == 0)
		return TEE_SUCCESS;

	res = alloc_temp_sec_mem(req_mem, mobj_tmp, &dst);
	if (res != TEE_SUCCESS)
		return res;
	dst_offs = 0;

	for (n = 0; n < TEE_NUM_PARAMS; n++) {

		if (!ta_private_memref[n])
			continue;

		s = ROUNDUP(param->u[n].mem.size, sizeof(uint32_t));

		switch (TEE_PARAM_TYPE_GET(param->types, n)) {
		case TEE_PARAM_TYPE_MEMREF_INPUT:
		case TEE_PARAM_TYPE_MEMREF_INOUT:
			va = (void *)param->u[n].mem.offs;
			if (va) {
				res = tee_svc_copy_from_user(dst, va,
						param->u[n].mem.size);
				if (res != TEE_SUCCESS)
					return res;
				param->u[n].mem.offs = dst_offs;
				param->u[n].mem.mobj = *mobj_tmp;
				tmp_buf_va[n] = dst;
				dst += s;
				dst_offs += s;
			}
			break;

		case TEE_PARAM_TYPE_MEMREF_OUTPUT:
			va = (void *)param->u[n].mem.offs;
			if (va) {
				param->u[n].mem.offs = dst_offs;
				param->u[n].mem.mobj = *mobj_tmp;
				tmp_buf_va[n] = dst;
				dst += s;
				dst_offs += s;
			}
			break;

		default:
			continue;
		}
	}

	return TEE_SUCCESS;
}
