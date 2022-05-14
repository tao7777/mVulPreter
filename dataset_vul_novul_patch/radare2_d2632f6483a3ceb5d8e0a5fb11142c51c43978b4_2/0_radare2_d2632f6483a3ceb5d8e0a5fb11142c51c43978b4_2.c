 static RList *r_bin_wasm_get_global_entries (RBinWasmObj *bin, RBinWasmSection *sec) {
 	RList *ret = NULL;
 	RBinWasmGlobalEntry *ptr = NULL;
 
 	if (!(ret = r_list_newf ((RListFree)free))) {
 		return NULL;
 	}
 
 	ut8* buf = bin->buf->buf + (ut32)sec->payload_data;
	int buflen = bin->buf->length - (ut32)sec->payload_data;
 	ut32 len =  sec->payload_len;
 	ut32 count = sec->count;
 	ut32 i = 0, r = 0;

	while (i < len && len < buflen && r < count) {
		if (!(ptr = R_NEW0 (RBinWasmGlobalEntry))) {
			return ret;
		}

		if (len + 8 > buflen || !(consume_u8 (buf + i, buf + len, (ut8*)&ptr->content_type, &i))) {
			goto beach;
		}
		if (len + 8 > buflen || !(consume_u8 (buf + i, buf + len, &ptr->mutability, &i))) {
			goto beach;
		}
		if (len + 8 > buflen || !(consume_init_expr (buf + i, buf + len, R_BIN_WASM_END_OF_CODE, NULL, &i))) {
			goto beach;
		}
		r_list_append (ret, ptr);
		r++;
	}
	return ret;
beach:
	free (ptr);
	return ret;
 }
