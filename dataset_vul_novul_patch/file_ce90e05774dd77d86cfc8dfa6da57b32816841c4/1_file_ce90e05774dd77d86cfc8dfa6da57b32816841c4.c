file_ms_alloc(int flags)
{
	struct magic_set *ms;
	size_t i, len;

	if ((ms = CAST(struct magic_set *, calloc((size_t)1,
	    sizeof(struct magic_set)))) == NULL)
		return NULL;

	if (magic_setflags(ms, flags) == -1) {
		errno = EINVAL;
		goto free;
	}

	ms->o.buf = ms->o.pbuf = NULL;
	len = (ms->c.len = 10) * sizeof(*ms->c.li);

	if ((ms->c.li = CAST(struct level_info *, malloc(len))) == NULL)
		goto free;

	ms->event_flags = 0;
	ms->error = -1;
	for (i = 0; i < MAGIC_SETS; i++)
		ms->mlist[i] = NULL;
	ms->file = "unknown";
	ms->line = 0;
	ms->indir_max = FILE_INDIR_MAX;
 	ms->name_max = FILE_NAME_MAX;
 	ms->elf_shnum_max = FILE_ELF_SHNUM_MAX;
 	ms->elf_phnum_max = FILE_ELF_PHNUM_MAX;
 	return ms;
 free:
 	free(ms);
	return NULL;
}
