struct svc_rdma_req_map *svc_rdma_get_req_map(struct svcxprt_rdma *xprt)
{
	struct svc_rdma_req_map *map = NULL;
	spin_lock(&xprt->sc_map_lock);
	if (list_empty(&xprt->sc_maps))
		goto out_empty;
	map = list_first_entry(&xprt->sc_maps,
			       struct svc_rdma_req_map, free);
	list_del_init(&map->free);
	spin_unlock(&xprt->sc_map_lock);
out:
	map->count = 0;
	return map;
out_empty:
	spin_unlock(&xprt->sc_map_lock);
	/* Pre-allocation amount was incorrect */
	map = alloc_req_map(GFP_NOIO);
	if (map)
		goto out;
	WARN_ONCE(1, "svcrdma: empty request map list?\n");
	return NULL;
}
