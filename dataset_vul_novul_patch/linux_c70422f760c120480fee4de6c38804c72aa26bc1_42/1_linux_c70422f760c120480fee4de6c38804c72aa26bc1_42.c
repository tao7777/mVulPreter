static struct svc_rdma_req_map *alloc_req_map(gfp_t flags)
{
	struct svc_rdma_req_map *map;
	map = kmalloc(sizeof(*map), flags);
	if (map)
		INIT_LIST_HEAD(&map->free);
	return map;
}
