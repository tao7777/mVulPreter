static int __init xfrm6_tunnel_spi_init(void)
{
	xfrm6_tunnel_spi_kmem = kmem_cache_create("xfrm6_tunnel_spi",
						  sizeof(struct xfrm6_tunnel_spi),
						  0, SLAB_HWCACHE_ALIGN,
						  NULL);
	if (!xfrm6_tunnel_spi_kmem)
		return -ENOMEM;
	return 0;
}
