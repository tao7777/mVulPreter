TEE_Result syscall_cryp_obj_populate(unsigned long obj,
			struct utee_attribute *usr_attrs,
			unsigned long attr_count)
{
	TEE_Result res;
	struct tee_ta_session *sess;
	struct tee_obj *o;
	const struct tee_cryp_obj_type_props *type_props;
	TEE_Attribute *attrs = NULL;

	res = tee_ta_get_current_session(&sess);
	if (res != TEE_SUCCESS)
		return res;

	res = tee_obj_get(to_user_ta_ctx(sess->ctx),
			  tee_svc_uref_to_vaddr(obj), &o);
	if (res != TEE_SUCCESS)
		return res;

	/* Must be a transient object */
	if ((o->info.handleFlags & TEE_HANDLE_FLAG_PERSISTENT) != 0)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Must not be initialized already */
	if ((o->info.handleFlags & TEE_HANDLE_FLAG_INITIALIZED) != 0)
		return TEE_ERROR_BAD_PARAMETERS;

	type_props = tee_svc_find_type_props(o->info.objectType);
 	if (!type_props)
 		return TEE_ERROR_NOT_IMPLEMENTED;
 
	attrs = malloc(sizeof(TEE_Attribute) * attr_count);
 	if (!attrs)
 		return TEE_ERROR_OUT_OF_MEMORY;
 	res = copy_in_attrs(to_user_ta_ctx(sess->ctx), usr_attrs, attr_count,
 			    attrs);
 	if (res != TEE_SUCCESS)
		goto out;

	res = tee_svc_cryp_check_attr(ATTR_USAGE_POPULATE, type_props,
				      attrs, attr_count);
	if (res != TEE_SUCCESS)
		goto out;

	res = tee_svc_cryp_obj_populate_type(o, type_props, attrs, attr_count);
	if (res == TEE_SUCCESS)
		o->info.handleFlags |= TEE_HANDLE_FLAG_INITIALIZED;

out:
	free(attrs);
	return res;
}
