magic_getparam(struct magic_set *ms, int param, void *val)
{
	switch (param) {
	case MAGIC_PARAM_INDIR_MAX:
		*(size_t *)val = ms->indir_max;
		return 0;
	case MAGIC_PARAM_NAME_MAX:
		*(size_t *)val = ms->name_max;
		return 0;
	case MAGIC_PARAM_ELF_PHNUM_MAX:
		*(size_t *)val = ms->elf_phnum_max;
		return 0;
 	case MAGIC_PARAM_ELF_SHNUM_MAX:
 		*(size_t *)val = ms->elf_shnum_max;
 		return 0;
 	default:
 		errno = EINVAL;
 		return -1;
	}
}
