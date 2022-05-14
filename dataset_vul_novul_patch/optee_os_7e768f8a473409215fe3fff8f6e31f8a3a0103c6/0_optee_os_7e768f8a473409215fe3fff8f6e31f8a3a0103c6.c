static struct mobj *alloc_ta_mem(size_t size)
{
#ifdef CFG_PAGED_USER_TA
	return mobj_paged_alloc(size);
 #else
 	struct mobj *mobj = mobj_mm_alloc(mobj_sec_ddr, size, &tee_mm_sec_ddr);
 
	if (mobj) {
		size_t granularity = BIT(tee_mm_sec_ddr.shift);

		/* Round up to allocation granularity size */
		memset(mobj_get_va(mobj, 0), 0, ROUNDUP(size, granularity));
	}
 	return mobj;
 #endif
 }
