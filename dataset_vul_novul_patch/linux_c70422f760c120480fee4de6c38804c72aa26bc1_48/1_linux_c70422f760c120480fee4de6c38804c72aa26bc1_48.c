void svc_rdma_put_req_map(struct svcxprt_rdma *xprt,
			  struct svc_rdma_req_map *map)
{
	spin_lock(&xprt->sc_map_lock);
	list_add(&map->free, &xprt->sc_maps);
	spin_unlock(&xprt->sc_map_lock);
}
