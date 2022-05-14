 static TEE_Result set_rmem_param(const struct optee_msg_param_rmem *rmem,
 				 struct param_mem *mem)
 {
 	uint64_t shm_ref = READ_ONCE(rmem->shm_ref);
 
 	mem->mobj = mobj_reg_shm_get_by_cookie(shm_ref);
	if (!mem->mobj)
		return TEE_ERROR_BAD_PARAMETERS;

 	mem->offs = READ_ONCE(rmem->offs);
 	mem->size = READ_ONCE(rmem->size);
 
 	return TEE_SUCCESS;
 }
