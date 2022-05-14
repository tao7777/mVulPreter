static  RFlagsAtOffset* r_flag_get_nearest_list(RFlag *f, ut64 off, int dir) {
static RFlagsAtOffset* r_flag_get_nearest_list(RFlag *f, ut64 off, int dir) {
 	RFlagsAtOffset *flags = NULL;
 	RFlagsAtOffset key;
 	key.off = off;
	if (dir >= 0) {
		flags = r_skiplist_get_geq (f->by_off, &key);
	} else {
		flags = r_skiplist_get_leq (f->by_off, &key);
	}
	if (dir == 0 && flags && flags->off != off) {
		return NULL;
	}
	return flags;
}
