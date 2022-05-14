static void xfrm6_tunnel_spi_fini(void)
{
	kmem_cache_destroy(xfrm6_tunnel_spi_kmem);
}
