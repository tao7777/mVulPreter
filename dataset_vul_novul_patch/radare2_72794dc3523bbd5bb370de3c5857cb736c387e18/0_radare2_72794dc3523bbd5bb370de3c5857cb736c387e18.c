static void *load_bytes(RBinFile *arch, const ut8 *buf, ut64 sz, ut64 loaddr, Sdb *sdb) {
	if (!buf || !sz || sz == UT64_MAX) {
 		return NULL;
 	}
 	RBuffer *tbuf = r_buf_new ();
	if (!tbuf) {
		return NULL;
	}
 	r_buf_set_bytes (tbuf, buf, sz);
 	struct r_bin_bflt_obj *res = r_bin_bflt_new_buf (tbuf);
 	r_buf_free (tbuf);
	return res ? res : NULL;
}
