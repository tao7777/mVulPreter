static void buf_to_pages(const void *buf, size_t buflen,
		struct page **pages, unsigned int *pgbase)
{
	const void *p = buf;
	*pgbase = offset_in_page(buf);
	p -= *pgbase;
	while (p < buf + buflen) {
		*(pages++) = virt_to_page(p);
		p += PAGE_CACHE_SIZE;
	}
}
