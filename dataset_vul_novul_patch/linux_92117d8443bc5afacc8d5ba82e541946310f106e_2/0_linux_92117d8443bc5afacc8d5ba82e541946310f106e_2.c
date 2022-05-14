void bpf_map_inc(struct bpf_map *map, bool uref)
/* prog's and map's refcnt limit */
#define BPF_MAX_REFCNT 32768

struct bpf_map *bpf_map_inc(struct bpf_map *map, bool uref)
 {
	if (atomic_inc_return(&map->refcnt) > BPF_MAX_REFCNT) {
		atomic_dec(&map->refcnt);
		return ERR_PTR(-EBUSY);
	}
 	if (uref)
 		atomic_inc(&map->usercnt);
	return map;
 }
