 static RList *r_bin_wasm_get_data_entries (RBinWasmObj *bin, RBinWasmSection *sec) {
 	RList *ret = NULL;
 	RBinWasmDataEntry *ptr = NULL;
 
 	if (!(ret = r_list_newf ((RListFree)free))) {
 		return NULL;
 	}
 
 	ut8* buf = bin->buf->buf + (ut32)sec->payload_data;
	ut32 len =  sec->payload_len;
 	ut32 count = sec->count;
 	ut32 i = 0, r = 0;
 	size_t n = 0;
 
	while (i < len && r < count) {
 		if (!(ptr = R_NEW0 (RBinWasmDataEntry))) {
 			return ret;
 		}
 		if (!(consume_u32 (buf + i, buf + len, &ptr->index, &i))) {
			free (ptr);
			return ret;
 		}
 		if (!(n = consume_init_expr (buf + i, buf + len, R_BIN_WASM_END_OF_CODE, NULL, &i))) {
			free (ptr);
			return ret;
 		}
 		ptr->offset.len = n;
 		if (!(consume_u32 (buf + i, buf + len, &ptr->size, &i))) {	
			free (ptr);
			return ret;
 		}
 		ptr->data = sec->payload_data + i;
 
 		r_list_append (ret, ptr);
 
 		r += 1;
 
 	}
 	return ret;
 }
