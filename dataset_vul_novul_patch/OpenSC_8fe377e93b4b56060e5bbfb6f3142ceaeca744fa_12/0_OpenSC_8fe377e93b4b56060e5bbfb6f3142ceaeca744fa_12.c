auth_select_file(struct sc_card *card, const struct sc_path *in_path,
				 struct sc_file **file_out)
{
	struct sc_path path;
	struct sc_file *tmp_file = NULL;
	size_t offs, ii;
	int rv;

	LOG_FUNC_CALLED(card->ctx);
	assert(card != NULL && in_path != NULL);
 
 	memcpy(&path, in_path, sizeof(struct sc_path));
 
	if (!auth_current_df)
		return SC_ERROR_OBJECT_NOT_FOUND;

 	sc_log(card->ctx, "in_path; type=%d, path=%s, out %p",
 			in_path->type, sc_print_path(in_path), file_out);
 	sc_log(card->ctx, "current path; type=%d, path=%s",
			auth_current_df->path.type, sc_print_path(&auth_current_df->path));
	if (auth_current_ef)
		sc_log(card->ctx, "current file; type=%d, path=%s",
				auth_current_ef->path.type, sc_print_path(&auth_current_ef->path));

	if (path.type == SC_PATH_TYPE_PARENT || path.type == SC_PATH_TYPE_FILE_ID)   {
		sc_file_free(auth_current_ef);
		auth_current_ef = NULL;

		rv = iso_ops->select_file(card, &path, &tmp_file);
		LOG_TEST_RET(card->ctx, rv, "select file failed");
		if (!tmp_file)
			return SC_ERROR_OBJECT_NOT_FOUND;

		if (path.type == SC_PATH_TYPE_PARENT)   {
			memcpy(&tmp_file->path, &auth_current_df->path, sizeof(struct sc_path));
			if (tmp_file->path.len > 2)
				tmp_file->path.len -= 2;

			sc_file_free(auth_current_df);
			sc_file_dup(&auth_current_df, tmp_file);
		}
		else   {
			if (tmp_file->type == SC_FILE_TYPE_DF)   {
				sc_concatenate_path(&tmp_file->path, &auth_current_df->path, &path);

				sc_file_free(auth_current_df);
				sc_file_dup(&auth_current_df, tmp_file);
			}
			else   {
				sc_file_free(auth_current_ef);

				sc_file_dup(&auth_current_ef, tmp_file);
				sc_concatenate_path(&auth_current_ef->path, &auth_current_df->path, &path);
			}
		}
		if (file_out)
			sc_file_dup(file_out, tmp_file);

		sc_file_free(tmp_file);
	}
	else if (path.type == SC_PATH_TYPE_DF_NAME)   {
		rv = iso_ops->select_file(card, &path, NULL);
		if (rv)   {
			sc_file_free(auth_current_ef);
			auth_current_ef = NULL;
		}
		LOG_TEST_RET(card->ctx, rv, "select file failed");
	}
	else   {
		for (offs = 0; offs < path.len && offs < auth_current_df->path.len; offs += 2)
			if (path.value[offs] != auth_current_df->path.value[offs] ||
					path.value[offs + 1] != auth_current_df->path.value[offs + 1])
				break;

		sc_log(card->ctx, "offs %"SC_FORMAT_LEN_SIZE_T"u", offs);
		if (offs && offs < auth_current_df->path.len)   {
			size_t deep = auth_current_df->path.len - offs;

			sc_log(card->ctx, "deep %"SC_FORMAT_LEN_SIZE_T"u",
			       deep);
			for (ii=0; ii<deep; ii+=2)   {
				struct sc_path tmp_path;

				memcpy(&tmp_path, &auth_current_df->path,  sizeof(struct sc_path));
				tmp_path.type = SC_PATH_TYPE_PARENT;

				rv = auth_select_file (card, &tmp_path, file_out);
				LOG_TEST_RET(card->ctx, rv, "select file failed");
			}
		}

		if (path.len - offs > 0)   {
			struct sc_path tmp_path;

			memset(&tmp_path, 0, sizeof(struct sc_path));
			tmp_path.type = SC_PATH_TYPE_FILE_ID;
			tmp_path.len = 2;

			for (ii=0; ii < path.len - offs; ii+=2)   {
				memcpy(tmp_path.value, path.value + offs + ii, 2);

				rv = auth_select_file(card, &tmp_path, file_out);
				LOG_TEST_RET(card->ctx, rv, "select file failed");
			}
		}
		else if (path.len - offs == 0 && file_out)  {
			if (sc_compare_path(&path, &auth_current_df->path))
				sc_file_dup(file_out, auth_current_df);
			else  if (auth_current_ef)
				sc_file_dup(file_out, auth_current_ef);
			else
				LOG_TEST_RET(card->ctx, SC_ERROR_INTERNAL, "No current EF");
		}
	}

	LOG_FUNC_RETURN(card->ctx, 0);
}
