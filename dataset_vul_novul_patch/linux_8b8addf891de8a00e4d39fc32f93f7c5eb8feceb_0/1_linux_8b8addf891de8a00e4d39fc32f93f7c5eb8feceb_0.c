static unsigned long mmap_legacy_base(unsigned long rnd)
{
	if (mmap_is_ia32())
		return TASK_UNMAPPED_BASE;
	else
		return TASK_UNMAPPED_BASE + rnd;
}
