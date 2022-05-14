 static RList *r_bin_wasm_get_element_entries (RBinWasmObj *bin, RBinWasmSection *sec) {
 	RList *ret = NULL;
 	RBinWasmElementEntry *ptr = NULL;
 
	if (!(ret = r_list_newf ((RListFree)free))) {
		return NULL;
 	}
 
 	ut8* buf = bin->buf->buf + (ut32)sec->payload_data;
	int buflen = bin->buf->length - (ut32)sec->payload_data;
 	ut32 len =  sec->payload_len;
 	ut32 count = sec->count;
 	ut32 i = 0, r = 0;
 
	while (i < len && len < buflen && r < count) {
 		if (!(ptr = R_NEW0 (RBinWasmElementEntry))) {
 			return ret;
 		}
 		if (!(consume_u32 (buf + i, buf + len, &ptr->index, &i))) {
			goto beach;
 		}
 		if (!(consume_init_expr (buf + i, buf + len, R_BIN_WASM_END_OF_CODE, NULL, &i))) {
			goto beach;
 		}
 		if (!(consume_u32 (buf + i, buf + len, &ptr->num_elem, &i))) {
			goto beach;
 		}
 		ut32 j = 0;
		while (i < len && j < ptr->num_elem) {
 			ut32 e;
 			if (!(consume_u32 (buf + i, buf + len, &e, &i))) {
 				free (ptr);
 				return ret;
 			}
 		}
 		r_list_append (ret, ptr);
 
 		r += 1;
 	}
	return ret;
beach:
	free (ptr);
 	return ret;
 }
