static struct mobj *alloc_ta_mem(size_t size)
{
#ifdef CFG_PAGED_USER_TA
	return mobj_paged_alloc(size);
 #else
 	struct mobj *mobj = mobj_mm_alloc(mobj_sec_ddr, size, &tee_mm_sec_ddr);
 
	if (mobj)
		memset(mobj_get_va(mobj, 0), 0, size);
 	return mobj;
 #endif
 }
