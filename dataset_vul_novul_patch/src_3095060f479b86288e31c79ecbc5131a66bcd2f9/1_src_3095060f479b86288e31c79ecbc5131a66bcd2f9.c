mm_zalloc(struct mm_master *mm, u_int ncount, u_int size)
{
	if (size == 0 || ncount == 0 || ncount > SIZE_MAX / size)
		fatal("%s: mm_zalloc(%u, %u)", __func__, ncount, size);
	return mm_malloc(mm, size * ncount);
}
