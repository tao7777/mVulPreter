 struct r_bin_dyldcache_obj_t* r_bin_dyldcache_from_bytes_new(const ut8* buf, ut64 size) {
	struct r_bin_dyldcache_obj_t *bin = R_NEW0 (struct r_bin_dyldcache_obj_t);
	if (!bin) {
 		return NULL;
 	}
 	if (!buf) {
 		return r_bin_dyldcache_free (bin);
 	}
	bin->b = r_buf_new ();
	if (!bin->b || !r_buf_set_bytes (bin->b, buf, size)) {
 		return r_bin_dyldcache_free (bin);
 	}
 	if (!r_bin_dyldcache_init (bin)) {
		return r_bin_dyldcache_free (bin);
	}
	bin->size = size;
	return bin;
}
