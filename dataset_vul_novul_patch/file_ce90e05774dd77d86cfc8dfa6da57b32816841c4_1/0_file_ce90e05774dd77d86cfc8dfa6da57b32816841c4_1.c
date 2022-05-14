magic_setparam(struct magic_set *ms, int param, const void *val)
{
	switch (param) {
	case MAGIC_PARAM_INDIR_MAX:
		ms->indir_max = *(const size_t *)val;
		return 0;
	case MAGIC_PARAM_NAME_MAX:
		ms->name_max = *(const size_t *)val;
		return 0;
	case MAGIC_PARAM_ELF_PHNUM_MAX:
		ms->elf_phnum_max = *(const size_t *)val;
		return 0;
 	case MAGIC_PARAM_ELF_SHNUM_MAX:
 		ms->elf_shnum_max = *(const size_t *)val;
 		return 0;
	case MAGIC_PARAM_ELF_NOTES_MAX:
		ms->elf_notes_max = *(const size_t *)val;
		return 0;
 	default:
 		errno = EINVAL;
 		return -1;
	}
}
