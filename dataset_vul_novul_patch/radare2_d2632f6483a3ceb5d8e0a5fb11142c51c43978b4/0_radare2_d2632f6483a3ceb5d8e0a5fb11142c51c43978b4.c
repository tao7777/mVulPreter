 static size_t consume_init_expr (ut8 *buf, ut8 *max, ut8 eoc, void *out, ut32 *offset) {
 	ut32 i = 0;
 	while (buf + i < max && buf[i] != eoc) {
		i++;
 	}
 	if (buf[i] != eoc) {
 		return 0;
	}
	if (offset) {
		*offset += i + 1;
	}
	return i + 1;
}
