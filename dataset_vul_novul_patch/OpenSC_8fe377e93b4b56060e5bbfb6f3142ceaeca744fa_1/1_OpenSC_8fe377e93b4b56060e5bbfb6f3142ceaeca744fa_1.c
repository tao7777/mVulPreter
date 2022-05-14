authentic_process_fci(struct sc_card *card, struct sc_file *file,
		 const unsigned char *buf, size_t buflen)
{
	struct sc_context *ctx = card->ctx;
	size_t taglen;
	int rv;
	unsigned ii;
	const unsigned char *tag = NULL;
	unsigned char ops_DF[8] = {
		SC_AC_OP_CREATE, SC_AC_OP_DELETE, SC_AC_OP_CRYPTO, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};
	unsigned char ops_EF[8] = {
		SC_AC_OP_READ, SC_AC_OP_DELETE, SC_AC_OP_UPDATE, SC_AC_OP_RESIZE, 0xFF, 0xFF, 0xFF, 0xFF
	};

	LOG_FUNC_CALLED(ctx);

	tag = sc_asn1_find_tag(card->ctx,  buf, buflen, 0x6F, &taglen);
	if (tag != NULL) {
		sc_log(ctx, "  FCP length %"SC_FORMAT_LEN_SIZE_T"u", taglen);
		buf = tag;
		buflen = taglen;
	}

	tag = sc_asn1_find_tag(card->ctx,  buf, buflen, 0x62, &taglen);
	if (tag != NULL) {
		sc_log(ctx, "  FCP length %"SC_FORMAT_LEN_SIZE_T"u", taglen);
		buf = tag;
		buflen = taglen;
	}

	rv = iso_ops->process_fci(card, file, buf, buflen);
	LOG_TEST_RET(ctx, rv, "ISO parse FCI failed");

	if (!file->sec_attr_len)   {
		sc_log_hex(ctx, "ACLs not found in data", buf, buflen);
		sc_log(ctx, "Path:%s; Type:%X; PathType:%X", sc_print_path(&file->path), file->type, file->path.type);
		if (file->path.type == SC_PATH_TYPE_DF_NAME || file->type == SC_FILE_TYPE_DF)   {
			file->type = SC_FILE_TYPE_DF;
		}
		else   {
			LOG_TEST_RET(ctx, SC_ERROR_OBJECT_NOT_FOUND, "ACLs tag missing");
		}
 	}
 
 	sc_log_hex(ctx, "ACL data", file->sec_attr, file->sec_attr_len);
	for (ii = 0; ii < file->sec_attr_len / 2; ii++)  {
 		unsigned char op = file->type == SC_FILE_TYPE_DF ? ops_DF[ii] : ops_EF[ii];
 		unsigned char acl = *(file->sec_attr + ii*2);
 		unsigned char cred_id = *(file->sec_attr + ii*2 + 1);
		unsigned sc = acl * 0x100 + cred_id;

		sc_log(ctx, "ACL(%i) op 0x%X, acl %X:%X", ii, op, acl, cred_id);
		if (op == 0xFF)
			;
		else if (!acl && !cred_id)
			sc_file_add_acl_entry(file, op, SC_AC_NONE, 0);
		else if (acl == 0xFF)
			sc_file_add_acl_entry(file, op, SC_AC_NEVER, 0);
		else if (acl & AUTHENTIC_AC_SM_MASK)
			sc_file_add_acl_entry(file, op, SC_AC_SCB, sc);
		else if (cred_id)
			sc_file_add_acl_entry(file, op, SC_AC_CHV, cred_id);
		else
			sc_file_add_acl_entry(file, op, SC_AC_NEVER, 0);
	}

	LOG_FUNC_RETURN(ctx, 0);
}
