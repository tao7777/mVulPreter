static bool svc_rdma_prealloc_maps(struct svcxprt_rdma *xprt)
{
	unsigned int i;
	/* One for each receive buffer on this connection. */
	i = xprt->sc_max_requests;
	while (i--) {
		struct svc_rdma_req_map *map;
		map = alloc_req_map(GFP_KERNEL);
		if (!map) {
			dprintk("svcrdma: No memory for request map\n");
			return false;
		}
		list_add(&map->free, &xprt->sc_maps);
	}
	return true;
}
