static int rtecp_select_file(sc_card_t *card,
		const sc_path_t *in_path, sc_file_t **file_out)
{
	sc_file_t **file_out_copy, *file;
	int r;

	assert(card && card->ctx && in_path);
	switch (in_path->type)
	{
	case SC_PATH_TYPE_DF_NAME:
	case SC_PATH_TYPE_FROM_CURRENT:
	case SC_PATH_TYPE_PARENT:
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, SC_ERROR_NOT_SUPPORTED);
	}
	assert(iso_ops && iso_ops->select_file);
	file_out_copy = file_out;
	r = iso_ops->select_file(card, in_path, file_out_copy);
	if (r || file_out_copy == NULL)
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE, r);
	assert(file_out_copy);
	file = *file_out_copy;
	assert(file);
	if (file->sec_attr && file->sec_attr_len == SC_RTECP_SEC_ATTR_SIZE)
 		set_acl_from_sec_attr(card, file);
 	else
 		r = SC_ERROR_UNKNOWN_DATA_RECEIVED;
	if (r && !file_out)
 		sc_file_free(file);
 	else
 	{
		assert(file_out);
		*file_out = file;
	}
	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE, r);
}
